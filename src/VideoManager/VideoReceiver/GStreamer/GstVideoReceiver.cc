/****************************************************************************
 *
 * (c) 2009-2024 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

//-----------------------------------------------------------------------------
// Our pipeline look like this:
//
//              +-->queue-->_decoderValve[-->_decoder-->_videoSink]
//              |
// _source-->_tee
//              |
//              +-->queue-->_recorderValve[-->_fileSink]
//-----------------------------------------------------------------------------

#include "GstVideoReceiver.h"
#include "GStreamerHelpers.h"
#include "QGCLoggingCategory.h"

#include <QtCore/QDateTime>
#include <QtCore/QUrl>
#include <QtQuick/QQuickItem>

#include <gst/gst.h>

QGC_LOGGING_CATEGORY(GstVideoReceiverLog, "qgc.videomanager.videoreceiver.gstreamer.gstvideoreceiver")

GstVideoReceiver::GstVideoReceiver(QObject *parent)
    : VideoReceiver(parent)
    , _worker(new GstVideoWorker(this))
{
    // qCDebug(GstVideoReceiverLog) << this;

    _worker->start();
    (void) connect(&_watchdogTimer, &QTimer::timeout, this, &GstVideoReceiver::_watchdog);
    _watchdogTimer.start(1000);
}

GstVideoReceiver::~GstVideoReceiver()
{
    stop();

    // Clean up video sink if still held (it's now preserved across stop/start cycles)
    if (_videoSink) {
        gst_clear_object(&_videoSink);
    }

    _worker->shutdown();

    // qCDebug(GstVideoReceiverLog) << this;
}

void GstVideoReceiver::start(uint32_t timeout)
{
    if (_needDispatch()) {
        _worker->dispatch([this, timeout]() { start(timeout); });
        return;
    }

    if (_pipeline) {
        qCDebug(GstVideoReceiverLog) << "Already running!" << _uri;
        _dispatchSignal([this]() { emit onStartComplete(STATUS_INVALID_STATE); });
        return;
    }

    if (_uri.isEmpty()) {
        qCDebug(GstVideoReceiverLog) << "Failed because URI is not specified";
        _dispatchSignal([this]() { emit onStartComplete(STATUS_INVALID_URL); });
        return;
    }

    _timeout = timeout;
    _buffer = lowLatency() ? -1 : 0;

    qCDebug(GstVideoReceiverLog) << "Starting" << _uri << ", lowLatency" << lowLatency() << ", timeout" << _timeout;

    _endOfStream = false;

    bool running = false;
    bool pipelineUp = false;

    GstElement *decoderQueue = nullptr;
    GstElement *recorderQueue = nullptr;

    do {
        _tee = gst_element_factory_make("tee", nullptr);
        if (!_tee)  {
            qCCritical(GstVideoReceiverLog) << "gst_element_factory_make('tee') failed";
            break;
        }

        GstPad *pad = gst_element_get_static_pad(_tee, "sink");
        if (!pad) {
            qCCritical(GstVideoReceiverLog) << "gst_element_get_static_pad() failed";
            break;
        }

        _lastSourceFrameTime = 0;

        _teeProbeId = gst_pad_add_probe(pad, GST_PAD_PROBE_TYPE_BUFFER, _teeProbe, this, nullptr);
        gst_clear_object(&pad);

        decoderQueue = gst_element_factory_make("queue", nullptr);
        if (!decoderQueue)  {
            qCCritical(GstVideoReceiverLog) << "gst_element_factory_make('queue') failed";
            break;
        }

        _decoderValve = gst_element_factory_make("valve", nullptr);
        if (!_decoderValve)  {
            qCCritical(GstVideoReceiverLog) << "gst_element_factory_make('valve') failed";
            break;
        }

        g_object_set(_decoderValve,
                     "drop", TRUE,
                     nullptr);

        recorderQueue = gst_element_factory_make("queue", nullptr);
        if (!recorderQueue)  {
            qCCritical(GstVideoReceiverLog) << "gst_element_factory_make('queue') failed";
            break;
        }

        _recorderValve = gst_element_factory_make("valve", nullptr);
        if (!_recorderValve) {
            qCCritical(GstVideoReceiverLog) << "gst_element_factory_make('valve') failed";
            break;
        }

        g_object_set(_recorderValve,
                     "drop", TRUE,
                     nullptr);

        _pipeline = gst_pipeline_new("receiver");
        if (!_pipeline) {
            qCCritical(GstVideoReceiverLog) << "gst_pipeline_new() failed";
            break;
        }

        g_object_set(_pipeline,
                     "message-forward", TRUE,
                     nullptr);

        _source = _makeSource(_uri);
        if (!_source) {
            qCCritical(GstVideoReceiverLog) << "_makeSource() failed";
            break;
        }

        gst_bin_add_many(GST_BIN(_pipeline), _source, _tee, decoderQueue, _decoderValve, recorderQueue, _recorderValve, nullptr);

        pipelineUp = true;

        GstPad *srcPad = nullptr;
        GstIterator *it = gst_element_iterate_src_pads(_source);
        GValue vpad = G_VALUE_INIT;
        switch (gst_iterator_next(it, &vpad)) {
            case GST_ITERATOR_OK:
                srcPad = GST_PAD(g_value_get_object(&vpad));
                (void) gst_object_ref(srcPad);
                (void) g_value_reset(&vpad);
                break;
            case GST_ITERATOR_RESYNC:
                gst_iterator_resync(it);
                break;
            default:
                break;
        }
        g_value_unset(&vpad);
        gst_iterator_free(it);

        if (srcPad) {
            _onNewSourcePad(srcPad);
            gst_clear_object(&srcPad);
        } else {
            (void) g_signal_connect(_source, "pad-added", G_CALLBACK(_onNewPad), this);
        }

        if (!gst_element_link_many(_tee, decoderQueue, _decoderValve, nullptr)) {
            qCCritical(GstVideoReceiverLog) << "Unable to link decoder queue";
            break;
        }

        if (!gst_element_link_many(_tee, recorderQueue, _recorderValve, nullptr)) {
            qCCritical(GstVideoReceiverLog) << "Unable to link recorder queue";
            break;
        }

        GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(_pipeline));
        if (bus) {
            gst_bus_enable_sync_message_emission(bus);
            (void) g_signal_connect(bus, "sync-message", G_CALLBACK(_onBusMessage), this);
            gst_clear_object(&bus);
        }

        GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(_pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline-initial");
        running = (gst_element_set_state(_pipeline, GST_STATE_PLAYING) != GST_STATE_CHANGE_FAILURE);
    } while(0);

    if (!running) {
        qCCritical(GstVideoReceiverLog) << "Failed";

        if (_pipeline) {
            (void) gst_element_set_state(_pipeline, GST_STATE_NULL);
            gst_clear_object(&_pipeline);
        }

        if (!pipelineUp) {
            gst_clear_object(&_recorderValve);
            gst_clear_object(&recorderQueue);
            gst_clear_object(&_decoderValve);
            gst_clear_object(&decoderQueue);
            gst_clear_object(&_tee);
            gst_clear_object(&_source);
        }

        // Rate limit restarts on failure. This sleep is OK because we're in the video worker thread.
        QThread::sleep(1);
        _dispatchSignal([this]() { emit onStartComplete(STATUS_FAIL); });
    } else {
        GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(_pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline-started");
        qCDebug(GstVideoReceiverLog) << "Started" << _uri;

        _dispatchSignal([this]() { emit onStartComplete(STATUS_OK); });
    }
}

void GstVideoReceiver::stop()
{
    if (_needDispatch()) {
        _worker->dispatch([this]() { stop(); });
        return;
    }

    if (_uri.isEmpty()) {
        qCWarning(GstVideoReceiverLog) << "Stop called on empty URI";
        return;
    }

    qCDebug(GstVideoReceiverLog) << "Stopping" << _uri;

    if (_teeProbeId != 0) {
        GstPad *sinkpad = gst_element_get_static_pad(_tee, "sink");
        if (sinkpad) {
            gst_pad_remove_probe(sinkpad, _teeProbeId);
            gst_clear_object(&sinkpad);
        }
        _teeProbeId = 0;
    }

    if (_pipeline) {
        GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(_pipeline));
        if (bus) {
            gst_bus_disable_sync_message_emission(bus);
            (void) g_signal_handlers_disconnect_by_data(bus, this);

            gboolean recordingValveClosed = TRUE;
            g_object_get(_recorderValve, "drop", &recordingValveClosed, nullptr);

            if (!recordingValveClosed) {
                (void) gst_element_send_event(_pipeline, gst_event_new_eos());

                GstMessage *msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, (GstMessageType)(GST_MESSAGE_EOS | GST_MESSAGE_ERROR));
                if (msg) {
                    switch (GST_MESSAGE_TYPE(msg)) {
                    case GST_MESSAGE_EOS:
                        qCDebug(GstVideoReceiverLog) << "End of stream received!";
                        break;
                    case GST_MESSAGE_ERROR:
                        qCCritical(GstVideoReceiverLog) << "Error stopping pipeline!";
                        break;
                    default:
                        break;
                    }

                    gst_clear_message(&msg);
                } else {
                    qCCritical(GstVideoReceiverLog) << "gst_bus_timed_pop_filtered() failed";
                }
            }

            gst_clear_object(&bus);
        } else {
            qCCritical(GstVideoReceiverLog) << "gst_pipeline_get_bus() failed";
        }

        (void) gst_element_set_state(_pipeline, GST_STATE_NULL);

        // FIXME: check if branch is connected and remove all elements from branch
        if (_fileSink) {
           _shutdownRecordingBranch();
        }

        if (_videoSink) {
            _shutdownDecodingBranch();
        }

        GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(_pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline-stopped");

        gst_clear_object(&_pipeline);
        _pipeline = nullptr;

        _recorderValve = nullptr;
        _decoderValve = nullptr;
        _tee = nullptr;
        _source = nullptr;

        _lastSourceFrameTime = 0;

        if (_streaming) {
            _streaming = false;
            qCDebug(GstVideoReceiverLog) << "Streaming stopped" << _uri;
            _dispatchSignal([this]() { emit streamingChanged(_streaming); });
        } else {
            qCDebug(GstVideoReceiverLog) << "Streaming did not start" << _uri;
        }
    }

    qCDebug(GstVideoReceiverLog) << "Stopped" << _uri;

    _dispatchSignal([this]() { emit onStopComplete(STATUS_OK); });
}

void GstVideoReceiver::startDecoding(void *sink)
{
    if (!sink) {
        qCCritical(GstVideoReceiverLog) << "VideoSink is NULL" << _uri;
        return;
    }

    if (_needDispatch()) {
        _worker->dispatch([this, sink]() mutable { startDecoding(sink); });
        return;
    }

    qCDebug(GstVideoReceiverLog) << "Starting decoding" << _uri;

    // Clear the decoding error flag now that we're on the worker thread.
    // All pending bus error recovery lambdas have been processed by this point
    // (they were queued before us). This allows future GL errors to trigger recovery.
    _decodingErrorPending = false;

    if (!_widget) {
        qCDebug(GstVideoReceiverLog) << "Video Widget is NULL" << _uri;
        _dispatchSignal([this]() { emit onStartDecodingComplete(STATUS_FAIL); });
        return;
    }

    // Only check if we're actively decoding, not whether _videoSink exists.
    // The video sink may exist from a previous run and we want to reuse it.
    if (_decoding) {
        qCDebug(GstVideoReceiverLog) << "Already decoding!" << _uri;
        _dispatchSignal([this]() { emit onStartDecodingComplete(STATUS_INVALID_STATE); });
        return;
    }

    GstElement *videoSink = GST_ELEMENT(sink);

    // If we already have a video sink and it's the same one, reuse it
    // If it's different, release the old one first
    if (_videoSink && _videoSink != videoSink) {
        qCDebug(GstVideoReceiverLog) << "Releasing old video sink, using new one" << _uri;
        gst_clear_object(&_videoSink);
    }

    GstPad *pad = gst_element_get_static_pad(videoSink, "sink");
    if (!pad) {
        qCCritical(GstVideoReceiverLog) << "Unable to find sink pad of video sink" << _uri;
        _dispatchSignal([this]() { emit onStartDecodingComplete(STATUS_FAIL); });
        return;
    }

    _lastVideoFrameTime = 0;
    _resetVideoSink = true;

    _videoSinkProbeId = gst_pad_add_probe(pad, GST_PAD_PROBE_TYPE_BUFFER, _videoSinkProbe, this, nullptr);
    gst_clear_object(&pad);

    // Only ref the sink if we don't already have a reference to it
    if (_videoSink != videoSink) {
        _videoSink = videoSink;
        gst_object_ref(_videoSink);
    }

    _removingDecoder = false;

    if (!_streaming) {
        _dispatchSignal([this]() { emit onStartDecodingComplete(STATUS_OK); });
        return;
    }

    if (!_addDecoder(_decoderValve)) {
        qCCritical(GstVideoReceiverLog) << "_addDecoder() failed" << _uri;
        _dispatchSignal([this]() { emit onStartDecodingComplete(STATUS_FAIL); });
        return;
    }

    g_object_set(_decoderValve,
                 "drop", FALSE,
                 nullptr);

    // NOTE: The post-decoder valve opening is scheduled from _addVideoSink,
    // not here, because the OMX decoder has a dynamic src pad — _addVideoSink
    // is called from pad-added on the streaming thread after startDecoding returns.

    qCDebug(GstVideoReceiverLog) << "Decoding started" << _uri;

    _dispatchSignal([this]() { emit onStartDecodingComplete(STATUS_OK); });
}

void GstVideoReceiver::stopDecoding()
{
    if (_needDispatch()) {
        _worker->dispatch([this]() { stopDecoding(); });
        return;
    }

    qCDebug(GstVideoReceiverLog) << "Stopping decoding" << _uri;

    if (!_pipeline || !_decoding) {
        qCDebug(GstVideoReceiverLog) << "Not decoding!" << _uri;
        _dispatchSignal([this]() { emit onStopDecodingComplete(STATUS_INVALID_STATE); });
        return;
    }

    g_object_set(_decoderValve,
                 "drop", TRUE,
                 nullptr);

    _removingDecoder = true;

    const bool ret = _unlinkBranch(_decoderValve);

    // FIXME: it is much better to emit onStopDecodingComplete() after decoding is really stopped
    // (which happens later due to async design) but as for now it is also not so bad...
    _dispatchSignal([this, ret](){ emit onStopDecodingComplete(ret ? STATUS_OK : STATUS_FAIL); });
}

void GstVideoReceiver::suspendDecoding()
{
    // Close the valve IMMEDIATELY on the calling thread before dispatching
    // the heavy teardown work to the worker thread. g_object_set is thread-safe.
    // This prevents frames from reaching the old decoder during the ~200ms
    // dispatch latency, which would cause gl_sync failures that poison the
    // shared GL context and persist across decoder restarts.
    if (_decoderValve) {
        g_object_set(_decoderValve, "drop", TRUE, nullptr);
        qCDebug(GstVideoReceiverLog) << "Valve closed immediately (before worker dispatch)";
    }

    if (_needDispatch()) {
        _worker->dispatch([this]() { suspendDecoding(); });
        return;
    }

    qCDebug(GstVideoReceiverLog) << "Suspending decoding (keeping RTSP alive)"
                                 << "_decoder=" << (_decoder ? "yes" : "no")
                                 << "_decoding=" << _decoding
                                 << _uri;

    if (_decoder || _decoding) {
        // Valve already closed above, proceed to teardown
        _shutdownDecodingBranch();
        qCDebug(GstVideoReceiverLog) << "Decoding suspended successfully";
    } else {
        qCDebug(GstVideoReceiverLog) << "Nothing to suspend (no decoder, not decoding)";
    }
}

void GstVideoReceiver::restartDecoding(void *newSink)
{
    if (!newSink) {
        qCCritical(GstVideoReceiverLog) << "VideoSink is NULL" << _uri;
        return;
    }

    // Close valve immediately to stop frames reaching the old decoder
    // during worker dispatch latency. g_object_set is thread-safe.
    if (_decoderValve) {
        g_object_set(_decoderValve, "drop", TRUE, nullptr);
    }

    if (_needDispatch()) {
        _worker->dispatch([this, newSink]() mutable { restartDecoding(newSink); });
        return;
    }

    qCDebug(GstVideoReceiverLog) << "Restarting decoding atomically" << _uri;

    if (_decoder || _decoding) {
        // Valve already closed above, proceed to teardown
        _shutdownDecodingBranch();
    }

    // Now start decoding with the fresh sink (runs in same worker dispatch,
    // so no frames are missed between shutdown and startup).
    startDecoding(newSink);
}

void GstVideoReceiver::startRecording(const QString &videoFile, FILE_FORMAT format)
{
    if (_needDispatch()) {
        const QString cachedVideoFile = videoFile;
        _worker->dispatch([this, cachedVideoFile, format]() { startRecording(cachedVideoFile, format); });
        return;
    }

    qCDebug(GstVideoReceiverLog) << "Starting recording" << _uri;

    if (!_pipeline) {
        qCDebug(GstVideoReceiverLog) << "Streaming is not active!" << _uri;
        _dispatchSignal([this](){ emit onStartRecordingComplete(STATUS_INVALID_STATE); });
        return;
    }

    if (_recording) {
        qCDebug(GstVideoReceiverLog) << "Already recording!" << _uri;
        _dispatchSignal([this]() { emit onStartRecordingComplete(STATUS_INVALID_STATE); });
        return;
    }

    qCDebug(GstVideoReceiverLog) << "New video file:" << videoFile << _uri;

    _fileSink = _makeFileSink(videoFile, format);
    if (!_fileSink) {
        qCCritical(GstVideoReceiverLog) << "_makeFileSink() failed" << _uri;
        _dispatchSignal([this]() { emit onStartRecordingComplete(STATUS_FAIL); });
        return;
    }

    _removingRecorder = false;

    (void) gst_object_ref(_fileSink);

    gst_bin_add(GST_BIN(_pipeline), _fileSink);

    if (!gst_element_link(_recorderValve, _fileSink)) {
        qCCritical(GstVideoReceiverLog) << "Failed to link valve and file sink" << _uri;
        _dispatchSignal([this]() { emit onStartRecordingComplete(STATUS_FAIL); });
        return;
    }

    (void) gst_element_sync_state_with_parent(_fileSink);

    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(_pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline-with-filesink");

    // Install a probe on the recording branch to drop buffers until we hit our first keyframe
    // When we hit our first keyframe, we can offset the timestamps appropriately according to the first keyframe time
    // This will ensure the first frame is a keyframe at t=0, and decoding can begin immediately on playback
    GstPad *probepad = gst_element_get_static_pad(_recorderValve, "src");
    if (!probepad) {
        qCCritical(GstVideoReceiverLog) << "gst_element_get_static_pad() failed" << _uri;
        _dispatchSignal([this]() { emit onStartRecordingComplete(STATUS_FAIL); });
        return;
    }

    (void) gst_pad_add_probe(probepad, GST_PAD_PROBE_TYPE_BUFFER, _keyframeWatch, this, nullptr); // to drop the buffers until key frame is received
    gst_clear_object(&probepad);

    g_object_set(_recorderValve,
                 "drop", FALSE,
                 nullptr);

    _recordingOutput = videoFile;
    _recording = true;
    qCDebug(GstVideoReceiverLog) << "Recording started" << _uri;
    _dispatchSignal([this]() {
        emit onStartRecordingComplete(STATUS_OK);
        emit recordingChanged(_recording);
    });
}

void GstVideoReceiver::stopRecording()
{
    if (_needDispatch()) {
        _worker->dispatch([this]() { stopRecording(); });
        return;
    }

    qCDebug(GstVideoReceiverLog) << "Stopping recording" << _uri;

    if (!_pipeline || !_recording) {
        qCDebug(GstVideoReceiverLog) << "Not recording!" << _uri;
        _dispatchSignal([this]() { emit onStopRecordingComplete(STATUS_INVALID_STATE); });
        return;
    }

    g_object_set(_recorderValve,
                 "drop", TRUE,
                 nullptr);

    _removingRecorder = true;

    const bool ret = _unlinkBranch(_recorderValve);

    // FIXME: it is much better to emit onStopRecordingComplete() after recording is really stopped
    // (which happens later due to async design) but as for now it is also not so bad...
    _dispatchSignal([this, ret]() { emit onStopRecordingComplete(ret ? STATUS_OK : STATUS_FAIL); });
}

void GstVideoReceiver::takeScreenshot(const QString &imageFile)
{
    if (_needDispatch()) {
        const QString cachedImageFile = imageFile;
        _worker->dispatch([this, cachedImageFile]() { takeScreenshot(cachedImageFile); });
        return;
    }

    qCDebug(GstVideoReceiverLog) << "taking screenshot" << _uri;

    // FIXME: record screenshot here
    _dispatchSignal([this]() { emit onTakeScreenshotComplete(STATUS_NOT_IMPLEMENTED); });
}

void GstVideoReceiver::_watchdog()
{
    _worker->dispatch([this]() {
        if (!_pipeline) {
            return;
        }

        const qint64 now = QDateTime::currentSecsSinceEpoch();
        if (_lastSourceFrameTime == 0) {
            _lastSourceFrameTime = now;
        }

        qint64 elapsed = now - _lastSourceFrameTime;
        if (elapsed > _timeout) {
            qCDebug(GstVideoReceiverLog) << "Stream timeout, no frames for" << elapsed << _uri;
            _dispatchSignal([this]() { emit timeout(); });
            stop();
        }

        if (_decoding && !_removingDecoder) {
            if (_lastVideoFrameTime == 0) {
                _lastVideoFrameTime = now;
            }

            elapsed = now - _lastVideoFrameTime;
            if (elapsed > (_timeout * 2)) {
                qCDebug(GstVideoReceiverLog) << "Video decoder timeout, no frames for" << elapsed << _uri;
                _dispatchSignal([this]() { emit timeout(); });
                stop();
            }
        }
    });
}

void GstVideoReceiver::_handleEOS()
{
    if (!_pipeline) {
        return;
    }

    if (_endOfStream) {
        stop();
    } else if (_decoding && _removingDecoder) {
        _shutdownDecodingBranch();
    } else if (_recording && _removingRecorder) {
        _shutdownRecordingBranch();
    } /*else {
        qCWarning(GstVideoReceiverLog) << "Unexpected EOS!";
        stop();
    }*/
}

gboolean GstVideoReceiver::_filterParserCaps(GstElement *bin, GstPad *pad, GstElement *element, GstQuery *query, gpointer data)
{
    Q_UNUSED(bin); Q_UNUSED(pad); Q_UNUSED(element); Q_UNUSED(data)

    if (GST_QUERY_TYPE(query) != GST_QUERY_CAPS) {
        return FALSE;
    }

    GstCaps *srcCaps;
    gst_query_parse_caps(query, &srcCaps);
    if (!srcCaps || gst_caps_is_any(srcCaps)) {
        return FALSE;
    }

    GstCaps *sinkCaps = nullptr;
    GstCaps *filter = nullptr;
    GstStructure *structure = gst_caps_get_structure(srcCaps, 0);
    if (gst_structure_has_name(structure, "video/x-h265")) {
        filter = gst_caps_from_string("video/x-h265");
        if (gst_caps_can_intersect(srcCaps, filter)) {
            sinkCaps = gst_caps_from_string("video/x-h265,stream-format=hvc1");
        }
        gst_clear_caps(&filter);
    } else if (gst_structure_has_name(structure, "video/x-h264")) {
        filter = gst_caps_from_string("video/x-h264");
        if (gst_caps_can_intersect(srcCaps, filter)) {
            sinkCaps = gst_caps_from_string("video/x-h264,stream-format=avc");
        }
        gst_clear_caps(&filter);
    }

    if (sinkCaps) {
        gst_query_set_caps_result(query, sinkCaps);
        gst_clear_caps(&sinkCaps);
        return TRUE;
    }

    return FALSE;
}

GstElement *GstVideoReceiver::_makeSource(const QString &input)
{
    if (input.isEmpty()) {
        qCCritical(GstVideoReceiverLog) << "Failed because URI is not specified";
        return nullptr;
    }

    const QUrl sourceUrl(input);

    const bool isRtsp = sourceUrl.scheme().startsWith("rtsp", Qt::CaseInsensitive);
    const bool isUdp264 = input.contains("udp://", Qt::CaseInsensitive);
    const bool isUdp265 = input.contains("udp265://", Qt::CaseInsensitive);
    const bool isUdpMPEGTS = input.contains("mpegts://", Qt::CaseInsensitive);
    const bool isTcpMPEGTS = input.contains("tcp://", Qt::CaseInsensitive);

    GstElement *source = nullptr;
    GstElement *buffer = nullptr;
    GstElement *tsdemux = nullptr;
    GstElement *parser = nullptr;
    GstElement *bin = nullptr;
    GstElement *srcbin = nullptr;

    do {
        if (isRtsp) {
            if (!GStreamer::is_valid_rtsp_uri(input.toUtf8().constData())) {
                qCCritical(GstVideoReceiverLog) << "Invalid RTSP URI:" << input;
                break;
            }

            source = gst_element_factory_make("rtspsrc", "source");
            if (!source) {
                qCCritical(GstVideoReceiverLog) << "gst_element_factory_make('rtspsrc') failed";
                break;
            }

            g_object_set(source,
                         "location", input.toUtf8().constData(),
                         "latency", 25,
                         // Cap the internal rtpjitterbuffer at `latency` and drop instead of
                         // letting it grow. Without this, jitterbuffer latency is a floor that
                         // only ever increases under sender/receiver clock skew, which on some
                         // air unit firmware revisions (different RTC behaviour) causes video
                         // lag to accumulate monotonically over time.
                         "drop-on-latency", TRUE,
                         "protocols", 1,                  // UDP only (GST_RTSP_LOWER_TRANS_UDP)
                         "udp-timeout", G_GUINT64_CONSTANT(10000000), // 10 seconds (15000000 microseconds)
                         nullptr);
        } else if (isTcpMPEGTS) {
            source = gst_element_factory_make("tcpclientsrc", "source");
            if (!source) {
                qCCritical(GstVideoReceiverLog) << "gst_element_factory_make('tcpclientsrc') failed";
                break;
            }

            const QString host = sourceUrl.host();
            const quint16 port = sourceUrl.port();
            g_object_set(source,
                         "host", host.toUtf8().constData(),
                         "port", port,
                         nullptr);
        } else if (isUdp264 || isUdp265 || isUdpMPEGTS) {
            source = gst_element_factory_make("udpsrc", "source");
            if (!source) {
                qCCritical(GstVideoReceiverLog) << "gst_element_factory_make('udpsrc') failed";
                break;
            }

            const QString uri = QStringLiteral("udp://%1:%2").arg(sourceUrl.host(), QString::number(sourceUrl.port()));
            g_object_set(source,
                         "uri", uri.toUtf8().constData(),
                         nullptr);

            GstCaps *caps = nullptr;
            if (isUdp264) {
                caps = gst_caps_from_string("application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264");
                if (!caps) {
                    qCCritical(GstVideoReceiverLog) << "gst_caps_from_string() failed";
                    break;
                }
            } else if (isUdp265) {
                caps = gst_caps_from_string("application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H265");
                if (!caps) {
                    qCCritical(GstVideoReceiverLog) << "gst_caps_from_string() failed";
                    break;
                }
            }

            if (caps) {
                g_object_set(source,
                             "caps", caps,
                             nullptr);
                gst_clear_caps(&caps);
            }
        } else {
            qCDebug(GstVideoReceiverLog) << "URI is not recognized";
        }

        if (!source) {
            qCCritical(GstVideoReceiverLog) << "gst_element_factory_make() for data source failed";
            break;
        }

        bin = gst_bin_new("sourcebin");
        if (!bin) {
            qCCritical(GstVideoReceiverLog) << "gst_bin_new('sourcebin') failed";
            break;
        }

        parser = gst_element_factory_make("parsebin", "parser");
        if (!parser) {
            qCCritical(GstVideoReceiverLog) << "gst_element_factory_make('parsebin') failed";
            break;
        }

        (void) g_signal_connect(parser, "autoplug-query", G_CALLBACK(_filterParserCaps), nullptr);

        gst_bin_add_many(GST_BIN(bin), source, parser, nullptr);

        // FIXME: AV: Android does not determine MPEG2-TS via parsebin - have to explicitly state which demux to use
        // FIXME: AV: tsdemux handling is a bit ugly - let's try to find elegant solution for that later
        if (isTcpMPEGTS || isUdpMPEGTS) {
            tsdemux = gst_element_factory_make("tsdemux", nullptr);
            if (!tsdemux) {
                qCCritical(GstVideoReceiverLog) << "gst_element_factory_make('tsdemux') failed";
                break;
            }

            (void) gst_bin_add(GST_BIN(bin), tsdemux);

            if (!gst_element_link(source, tsdemux)) {
                qCCritical(GstVideoReceiverLog) << "gst_element_link() failed";
                break;
            }

            source = tsdemux;
            tsdemux = nullptr;
        }

        int probeRes = 0;
        (void) gst_element_foreach_src_pad(source, _padProbe, &probeRes);

        if (probeRes & 1) {
            if ((probeRes & 2) && (_buffer >= 0)) {
                buffer = gst_element_factory_make("rtpjitterbuffer", nullptr);
                if (!buffer) {
                    qCCritical(GstVideoReceiverLog) << "gst_element_factory_make('rtpjitterbuffer') failed";
                    break;
                }

                (void) gst_bin_add(GST_BIN(bin), buffer);

                if (!gst_element_link_many(source, buffer, parser, nullptr)) {
                    qCCritical(GstVideoReceiverLog) << "gst_element_link() failed";
                    break;
                }
            } else {
                if (!gst_element_link(source, parser)) {
                    qCCritical(GstVideoReceiverLog) << "gst_element_link() failed";
                    break;
                }
            }
        } else {
            (void) g_signal_connect(source, "pad-added", G_CALLBACK(_linkPad), parser);
        }

        (void) g_signal_connect(parser, "pad-added", G_CALLBACK(_wrapWithGhostPad), nullptr);

        source = tsdemux = buffer = parser = nullptr;

        srcbin = bin;
        bin = nullptr;
    } while(0);

    gst_clear_object(&bin);
    gst_clear_object(&parser);
    gst_clear_object(&tsdemux);
    gst_clear_object(&buffer);
    gst_clear_object(&source);

    return srcbin;
}

GstElement *GstVideoReceiver::_makeDecoder(GstCaps *caps, GstElement *videoSink)
{
    Q_UNUSED(caps); Q_UNUSED(videoSink)

    // Try OMX hardware decoder directly (like v4.0.8)
    qWarning() << "GstVideoReceiver::_makeDecoder() attempting to create OMX decoder";
    GstElement *omxDecoder = gst_element_factory_make("amcviddec-omxarmvideov5xxdecoder", "omx-decoder");
    if (omxDecoder) {
        qWarning() << "Successfully created OMX hardware decoder";
        return omxDecoder;
    } else {
        qWarning() << "OMX decoder not available, falling back to software decoder";
    }

    // Fallback to software decoder
    GstElement *decoder = gst_element_factory_make("avdec_h264", "h264-decoder");
    if (!decoder) {
        qWarning() << "VideoReceiver::start() failed. Error with gst_element_factory_make('avdec_h264')";
    } else {
        qWarning() << "Using software H.264 decoder";
    }

    return decoder;
}

GstElement *GstVideoReceiver::_makeFileSink(const QString &videoFile, FILE_FORMAT format)
{
    GstElement *fileSink = nullptr;
    GstElement *mux = nullptr;
    GstElement *sink = nullptr;
    GstElement *bin = nullptr;
    bool releaseElements = true;

    do {
        if (!isValidFileFormat(format)) {
            qCCritical(GstVideoReceiverLog) << "Unsupported file format";
            break;
        }

        mux = gst_element_factory_make(_kFileMux[format], nullptr);
        if (!mux) {
            qCCritical(GstVideoReceiverLog) << "gst_element_factory_make('" << _kFileMux[format] << "') failed";
            break;
        }

        sink = gst_element_factory_make("filesink", nullptr);
        if (!sink) {
            qCCritical(GstVideoReceiverLog) << "gst_element_factory_make('filesink') failed";
            break;
        }

        g_object_set(sink,
                     "location", qPrintable(videoFile),
                     nullptr);

        bin = gst_bin_new("sinkbin");
        if (!bin) {
            qCCritical(GstVideoReceiverLog) << "gst_bin_new('sinkbin') failed";
            break;
        }

        GstPadTemplate *padTemplate = gst_element_class_get_pad_template(GST_ELEMENT_GET_CLASS(mux), "video_%u");
        if (!padTemplate) {
            qCCritical(GstVideoReceiverLog) << "gst_element_class_get_pad_template(mux) failed";
            break;
        }

        // FIXME: pad handling is potentially leaking (and other similar places too!)
        GstPad *pad = gst_element_request_pad(mux, padTemplate, nullptr, nullptr);
        if (!pad) {
            qCCritical(GstVideoReceiverLog) << "gst_element_request_pad(mux) failed";
            break;
        }

        gst_bin_add_many(GST_BIN(bin), mux, sink, nullptr);

        releaseElements = false;

        GstPad *ghostpad = gst_ghost_pad_new("sink", pad);
        (void) gst_element_add_pad(bin, ghostpad);
        gst_clear_object(&pad);

        if (!gst_element_link(mux, sink)) {
            qCCritical(GstVideoReceiverLog) << "gst_element_link() failed";
            break;
        }

        fileSink = bin;
        bin = nullptr;
    } while(0);

    if (releaseElements) {
        gst_clear_object(&sink);
        gst_clear_object(&mux);
    }

    gst_clear_object(&bin);
    return fileSink;
}

void GstVideoReceiver::_onNewSourcePad(GstPad *pad)
{
    if (!gst_element_link(_source, _tee)) {
        qCCritical(GstVideoReceiverLog) << "Unable to link source";
        return;
    }

#if 0
    qWarning() << "GstVideoReceiver::_onNewSourcePad() called - checking pad caps";

    // Check pad caps to see what we're dealing with
    GstCaps *caps = gst_pad_query_caps(pad, nullptr);
    if (caps) {
        gchar *capsStr = gst_caps_to_string(caps);
        qWarning() << "GstVideoReceiver: Source pad caps:" << capsStr;
        g_free(capsStr);
        gst_caps_unref(caps);
    }

    // Check if tee sink is already linked
    GstPad *teeSink = gst_element_get_static_pad(_tee, "sink");
    if (!teeSink) {
        qCCritical(GstVideoReceiverLog) << "Unable to get tee sink pad";
        return;
    }

    if (gst_pad_is_linked(teeSink)) {
        qWarning() << "GstVideoReceiver: Tee sink already linked, ignoring additional pad";
        gst_object_unref(teeSink);
        return;
    }

    if (gst_pad_link(pad, teeSink) != GST_PAD_LINK_OK) {
        qCCritical(GstVideoReceiverLog) << "Unable to link source pad to tee";
        gst_object_unref(teeSink);
        return;
    }

    gst_object_unref(teeSink);
#endif

    if (!_streaming) {
        _streaming = true;
        qCDebug(GstVideoReceiverLog) << "Streaming started" << _uri;
        _dispatchSignal([this]() { emit streamingChanged(_streaming); });
    }

    (void) gst_pad_add_probe(pad, GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM, _eosProbe, this, nullptr);
    if (!_videoSink) {
        return;
    }

    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(_pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline-with-new-source-pad");

    if (!_addDecoder(_decoderValve)) {
        qCCritical(GstVideoReceiverLog) << "_addDecoder() failed";
        return;
    }

    g_object_set(_decoderValve,
                 "drop", FALSE,
                 nullptr);

    qCDebug(GstVideoReceiverLog) << "Decoding started" << _uri;
}

void GstVideoReceiver::_onNewDecoderPad(GstPad *pad)
{
    qCDebug(GstVideoReceiverLog) << "_onNewDecoderPad" << _uri;

    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(_pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline-with-new-decoder-pad");

    if (!_addVideoSink(pad)) {
        qCCritical(GstVideoReceiverLog) << "_addVideoSink() failed";
    }
}

bool GstVideoReceiver::_addDecoder(GstElement *src)
{
    GstPad *srcpad = gst_element_get_static_pad(src, "src");
    if (!srcpad) {
        qCCritical(GstVideoReceiverLog) << "gst_element_get_static_pad() failed";
        return false;
    }

    GstCaps *caps = gst_pad_query_caps(srcpad, nullptr);
    if (!caps) {
        qCCritical(GstVideoReceiverLog) << "gst_pad_query_caps() failed";
        gst_clear_object(&srcpad);
        return false;
    }

#if 0
    // Debug: show what caps the valve is outputting
    gchar *srcCapsStr = gst_caps_to_string(caps);
    qWarning() << "GstVideoReceiver: Valve output caps:" << srcCapsStr;
    g_free(srcCapsStr);
#endif

    gst_clear_object(&srcpad);

    _decoder = _makeDecoder();
    if (!_decoder) {
        qCCritical(GstVideoReceiverLog) << "_makeDecoder() failed";
        gst_clear_caps(&caps);
        return false;
    }

#if 0
    // Query what caps the OMX decoder actually accepts
    GstPad *decoderSink = gst_element_get_static_pad(_decoder, "sink");
    if (decoderSink) {
        GstCaps *decoderCaps = gst_pad_query_caps(decoderSink, nullptr);
        if (decoderCaps) {
            gchar *decoderCapsStr = gst_caps_to_string(decoderCaps);
            qWarning() << "GstVideoReceiver: OMX decoder accepts caps:" << decoderCapsStr;
            g_free(decoderCapsStr);
            gst_caps_unref(decoderCaps);
        }
        gst_object_unref(decoderSink);
    }
#endif

    (void) gst_object_ref(_decoder);

    gst_clear_caps(&caps);

    (void) gst_bin_add(GST_BIN(_pipeline), _decoder);
    (void) gst_element_sync_state_with_parent(_decoder);

    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(_pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline-with-decoder");

    // Add h264parse to convert AVC to byte-stream format for OMX decoder
    _h264parse = gst_element_factory_make("h264parse", nullptr);
    if (_h264parse) {
        // Configure h264parse to output byte-stream format
        g_object_set(_h264parse, "config-interval", -1, nullptr);

        (void) gst_object_ref(_h264parse);
        gst_bin_add(GST_BIN(_pipeline), _h264parse);
        gst_element_sync_state_with_parent(_h264parse);

        if (!gst_element_link_many(src, _h264parse, _decoder, nullptr)) {
            qCCritical(GstVideoReceiverLog) << "Unable to link decoder with h264parse";
            return false;
        }

        qWarning() << "GstVideoReceiver: Added h264parse to convert AVC to byte-stream for OMX decoder";
    } else {
        qCCritical(GstVideoReceiverLog) << "Failed to create h264parse element";
        return false;
    }

    GstPad *srcPad = nullptr;
    GstIterator *it = gst_element_iterate_src_pads(_decoder);
    GValue vpad = G_VALUE_INIT;
    switch (gst_iterator_next(it, &vpad)) {
        case GST_ITERATOR_OK:
            srcPad = GST_PAD(g_value_get_object(&vpad));
            (void) gst_object_ref(srcPad);
            (void) g_value_reset(&vpad);
            break;
        case GST_ITERATOR_RESYNC:
            gst_iterator_resync(it);
            break;
        default:
            break;
    }
    g_value_unset(&vpad);
    gst_iterator_free(it);

    if (srcPad) {
        _onNewDecoderPad(srcPad);
    } else {
        (void) g_signal_connect(_decoder, "pad-added", G_CALLBACK(_onNewPad), this);
    }

    gst_clear_object(&srcPad);
    return true;
}

bool GstVideoReceiver::_addVideoSink(GstPad *pad)
{
    GstCaps *caps = gst_pad_query_caps(pad, nullptr);

    // On restarts (_hasDecodedBefore=true), install a pad probe on the decoder's
    // src pad BEFORE linking to the video sink. The probe drops BOTH buffers AND
    // downstream events (including caps). Additionally, keep the video sink in
    // NULL state during the reconfiguration window to prevent ANY GL resource
    // allocation (even from upstream caps queries during state transitions).
    //
    // Without these protections, glcolorconvert creates an FBO with the initial
    // SurfaceTexture. When OMX output port reconfiguration recreates the
    // SurfaceTexture (~1.1s later), the FBO's texture attachment becomes invalid
    // → GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT.
    //
    // After the reconfiguration window (3s), we:
    // 1. Remove the probe → check_sticky re-forwards latest caps
    // 2. Sync the video sink state → PLAYING → GL resources created with
    //    the stable post-reconfiguration texture
    //
    // On initial startup (_hasDecodedBefore=false), the GL error doesn't occur.
    if (_hasDecodedBefore) {
        // pad IS the decoder's src pad (passed from _onNewDecoderPad)
        _postDecoderDropProbePad = GST_PAD(gst_object_ref(pad));
        _postDecoderDropProbeId = gst_pad_add_probe(
            _postDecoderDropProbePad,
            static_cast<GstPadProbeType>(GST_PAD_PROBE_TYPE_BUFFER | GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM),
            [](GstPad *, GstPadProbeInfo *, gpointer) -> GstPadProbeReturn {
                return GST_PAD_PROBE_DROP;
            },
            nullptr, nullptr);
        qWarning() << "GstVideoReceiver: Drop probe installed BEFORE link (blocking during OMX reconfiguration)";
    }

    (void) gst_object_ref(_videoSink); // gst_bin_add() will steal one reference
    (void) gst_bin_add(GST_BIN(_pipeline), _videoSink);

    if (!gst_element_link(_decoder, _videoSink)) {
        // Clean up probe if link fails
        if (_postDecoderDropProbeId != 0 && _postDecoderDropProbePad) {
            gst_pad_remove_probe(_postDecoderDropProbePad, _postDecoderDropProbeId);
            _postDecoderDropProbeId = 0;
            gst_clear_object(&_postDecoderDropProbePad);
        }
        (void) gst_bin_remove(GST_BIN(_pipeline), _videoSink);
        qCCritical(GstVideoReceiverLog) << "Unable to link decoder to video sink";
        gst_clear_caps(&caps);
        return false;
    }

    g_object_set(_videoSink,
                 "widget", _widget,
                 "sync", FALSE,
                 NULL);

    // Only sync video sink state immediately on initial startup.
    // On restarts, defer state sync until after reconfiguration window
    // to prevent GL resource allocation with pre-reconfiguration texture.
    if (!_hasDecodedBefore) {
        (void) gst_element_sync_state_with_parent(_videoSink);
    }

    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(_pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline-with-videosink");

    if (_decoderValve) {
        // Extracting video size from source is more guaranteed
        GstPad *valveSrcPad = gst_element_get_static_pad(_decoderValve, "src");
        const GstCaps *valveSrcPadCaps = gst_pad_query_caps(valveSrcPad, nullptr);
        const GstStructure *structure = gst_caps_get_structure(valveSrcPadCaps, 0);
        if (structure) {
            gint width, height;
            (void) gst_structure_get_int(structure, "width", &width);
            (void) gst_structure_get_int(structure, "height", &height);
            _dispatchSignal([this, width, height]() { emit videoSizeChanged(QSize(width, height)); });
        }
    } else {
        _dispatchSignal([this]() { emit videoSizeChanged(QSize()); });
    }

    // Schedule probe removal + video sink state sync after OMX reconfiguration
    // window (3s). The video sink stays in NULL state until then, preventing
    // any GL resource allocation with the pre-reconfiguration texture.
    if (_postDecoderDropProbeId != 0) {
        qWarning() << "GstVideoReceiver: Holding video sink in NULL state for 3s OMX reconfiguration window";
        _worker->dispatch([this]() {
            QThread::msleep(3000);
            if (_postDecoderDropProbeId != 0 && _postDecoderDropProbePad && _pipeline) {
                gst_pad_remove_probe(_postDecoderDropProbePad, _postDecoderDropProbeId);
                _postDecoderDropProbeId = 0;
                gst_clear_object(&_postDecoderDropProbePad);
                qWarning() << "GstVideoReceiver: Drop probe removed after reconfiguration window";
            }
            // NOW sync video sink state - GL resources will be created with
            // post-reconfiguration texture
            if (_videoSink && _pipeline) {
                (void) gst_element_sync_state_with_parent(_videoSink);
                qWarning() << "GstVideoReceiver: Video sink synced to PLAYING after reconfiguration window";
            }
        });
    }

    gst_clear_caps(&caps);
    return true;
}

void GstVideoReceiver::_noteTeeFrame()
{
    _lastSourceFrameTime = QDateTime::currentSecsSinceEpoch();
}

void GstVideoReceiver::_noteVideoSinkFrame()
{
    _lastVideoFrameTime = QDateTime::currentSecsSinceEpoch();
    _hasDecodedBefore = true;
    if (!_decoding) {
        _decoding = true;
        qCDebug(GstVideoReceiverLog) << "Decoding started";
        _dispatchSignal([this]() { emit decodingChanged(_decoding); });
    }
}

void GstVideoReceiver::_noteEndOfStream()
{
    _endOfStream = true;
}

bool GstVideoReceiver::_unlinkBranch(GstElement *from)
{
    GstPad *src = gst_element_get_static_pad(from, "src");
    if (!src) {
        qCCritical(GstVideoReceiverLog) << "gst_element_get_static_pad() failed";
        return false;
    }

    GstPad *sink = gst_pad_get_peer(src);
    if (!sink) {
        gst_clear_object(&src);
        qCCritical(GstVideoReceiverLog) << "gst_pad_get_peer() failed";
        return false;
    }

    if (!gst_pad_unlink(src, sink)) {
        gst_clear_object(&src);
        gst_clear_object(&sink);
        qCCritical(GstVideoReceiverLog) << "gst_pad_unlink() failed";
        return false;
    }

    gst_clear_object(&src);

    // Send EOS at the beginning of the branch
    const gboolean ret = gst_pad_send_event(sink, gst_event_new_eos());

    gst_clear_object(&sink);

    if (!ret) {
        qCCritical(GstVideoReceiverLog) << "Branch EOS was NOT sent";
        return false;
    }

    qCDebug(GstVideoReceiverLog) << "Branch EOS was sent";

    return true;
}

void GstVideoReceiver::_shutdownDecodingBranch()
{
    if (_h264parse) {
        GstObject *parent = gst_element_get_parent(_h264parse);
        if (parent) {
            (void) gst_bin_remove(GST_BIN(_pipeline), _h264parse);
            (void) gst_element_set_state(_h264parse, GST_STATE_NULL);
            gst_clear_object(&parent);
        }
        gst_clear_object(&_h264parse);
    }

    // Remove drop probe before destroying decoder (probe is on decoder's src pad)
    if (_postDecoderDropProbeId != 0 && _postDecoderDropProbePad) {
        gst_pad_remove_probe(_postDecoderDropProbePad, _postDecoderDropProbeId);
        _postDecoderDropProbeId = 0;
        gst_clear_object(&_postDecoderDropProbePad);
    }

    if (_decoder) {
        GstObject *parent = gst_element_get_parent(_decoder);
        if (parent) {
            (void) gst_bin_remove(GST_BIN(_pipeline), _decoder);
            (void) gst_element_set_state(_decoder, GST_STATE_NULL);
            gst_clear_object(&parent);
        }

        gst_clear_object(&_decoder);
    }

    if (_videoSinkProbeId != 0) {
        GstPad *sinkpad = gst_element_get_static_pad(_videoSink, "sink");
        if (sinkpad) {
            gst_pad_remove_probe(sinkpad, _videoSinkProbeId);
            gst_clear_object(&sinkpad);
        }
        _videoSinkProbeId = 0;
    }

    _lastVideoFrameTime = 0;

    GstObject *parent = gst_element_get_parent(_videoSink);
    if (parent) {
        // Remove from pipeline but do NOT destroy - we want to reuse it like old 4.0.8 did.
        // The old code would remove the sink from the pipeline but keep the reference,
        // allowing it to be re-added on the next start(). This preserves the GL context.
        (void) gst_bin_remove(GST_BIN(_pipeline), _videoSink);
        (void) gst_element_set_state(_videoSink, GST_STATE_NULL);
        gst_clear_object(&parent);
    }

    // NOTE: We intentionally do NOT call gst_clear_object(&_videoSink) here.
    // The video sink is kept and reused across stop/start cycles, just like in
    // the old 4.0.8 code. This preserves the GL context/textures associated with
    // the Qt widget and prevents black screen issues after HDMI/resolution changes.

    _removingDecoder = false;

    if (_decoding) {
        _decoding = false;
        qCDebug(GstVideoReceiverLog) << "Decoding stopped";
        _dispatchSignal([this]() { emit decodingChanged(_decoding); });
    }

    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(_pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline-decoding-stopped");
}

void GstVideoReceiver::_shutdownRecordingBranch()
{
    gst_bin_remove(GST_BIN(_pipeline), _fileSink);
    gst_element_set_state(_fileSink, GST_STATE_NULL);
    gst_clear_object(&_fileSink);

    _removingRecorder = false;

    if (_recording) {
        _recording = false;
        qCDebug(GstVideoReceiverLog) << "Recording stopped";
        _dispatchSignal([this]() { emit recordingChanged(_recording); });
    }

    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(_pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline-recording-stopped");
}

bool GstVideoReceiver::_needDispatch()
{
    return _worker->needDispatch();
}

void GstVideoReceiver::_dispatchSignal(Task emitter)
{
    _signalDepth += 1;

    // QElapsedTimer timer;
    // timer.start();

    emitter();

    // qCDebug(GstVideoReceiverLog) << "Task took" << timer.elapsed() << "ms";

    _signalDepth -= 1;
}

gboolean GstVideoReceiver::_onBusMessage(GstBus * /* bus */, GstMessage *msg, gpointer data)
{
    if (!msg || !data) {
        qCCritical(GstVideoReceiverLog) << "Invalid parameters in _onBusMessage: msg=" << msg << "data=" << data;
        return TRUE;
    }

    GstVideoReceiver *pThis = static_cast<GstVideoReceiver*>(data);

    switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_ERROR: {
        gchar *debug;
        GError *error;
        gst_message_parse_error(msg, &error, &debug);

        if (debug) {
            qCDebug(GstVideoReceiverLog) << "GStreamer debug:" << debug;
            g_clear_pointer(&debug, g_free);
        }

        if (error) {
            qCCritical(GstVideoReceiverLog) << "GStreamer error:" << error->message;
            g_clear_error(&error);
        }

        // Ignore errors from elements that have been removed from our pipeline.
        // After a decoding restart, stale error messages from destroyed elements
        // can arrive. Without this check, they'd trigger stop() and tear down RTSP.
        GstObject *errorSrc = GST_MESSAGE_SRC(msg);
        if (pThis->_pipeline && errorSrc &&
            !gst_object_has_as_ancestor(errorSrc, GST_OBJECT(pThis->_pipeline))) {
            qCDebug(GstVideoReceiverLog) << "Ignoring error from element no longer in pipeline";
            break;
        }

        // Check if the error came from the decoding branch (video sink or decoder).
        // GL errors (e.g. GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT during caps renegotiation)
        // come from elements inside the qgcvideosinkbin (_videoSink). For these errors,
        // we only restart the decoding branch instead of tearing down the entire pipeline
        // (which would send RTSP TEARDOWN, causing the air unit to destroy its encoder).
        bool isDecodingError = pThis->_decoding && errorSrc &&
            ((pThis->_videoSink &&
              (errorSrc == GST_OBJECT(pThis->_videoSink) ||
               gst_object_has_as_ancestor(errorSrc, GST_OBJECT(pThis->_videoSink)))) ||
             (pThis->_decoder &&
              (errorSrc == GST_OBJECT(pThis->_decoder) ||
               gst_object_has_as_ancestor(errorSrc, GST_OBJECT(pThis->_decoder)))));

        // Use atomic compare-exchange to coalesce multiple bus error messages
        // (from glcontextegl, glcolorconvert, omx-decoder) into a single recovery.
        // Without this, each element's error triggers a separate decodingError,
        // causing "Name 'omx-decoder' is not unique" when the second recovery
        // tries to create a decoder while the first is still in the pipeline.
        bool expected = false;
        if (isDecodingError && pThis->_decodingErrorPending.compare_exchange_strong(expected, true)) {
            pThis->_worker->dispatch([pThis]() {
                qCDebug(GstVideoReceiverLog) << "Decoding error - restarting decoding branch only (keeping RTSP alive)";
                if (pThis->_decoderValve) {
                    g_object_set(pThis->_decoderValve, "drop", TRUE, nullptr);
                }
                pThis->_shutdownDecodingBranch();
                pThis->_dispatchSignal([pThis]() { emit pThis->decodingError(); });
            });
        } else if (isDecodingError) {
            qCDebug(GstVideoReceiverLog) << "Ignoring duplicate decoding error (recovery already pending)";
        } else {
            pThis->_worker->dispatch([pThis]() {
                qCDebug(GstVideoReceiverLog) << "Stopping because of error";
                pThis->stop();
            });
        }
        break;
    }
    case GST_MESSAGE_EOS:
        pThis->_worker->dispatch([pThis]() {
            qCDebug(GstVideoReceiverLog) << "Received EOS";
            pThis->_handleEOS();
        });
        break;
    case GST_MESSAGE_ELEMENT: {
        const GstStructure *structure = gst_message_get_structure(msg);
        if (!gst_structure_has_name(structure, "GstBinForwarded")) {
            break;
        }

        GstMessage *forward_msg = nullptr;
        gst_structure_get(structure, "message", GST_TYPE_MESSAGE, &forward_msg, NULL);
        if (!forward_msg) {
            break;
        }

        if (GST_MESSAGE_TYPE(forward_msg) == GST_MESSAGE_EOS) {
            pThis->_worker->dispatch([pThis]() {
                qCDebug(GstVideoReceiverLog) << "Received branch EOS";
                pThis->_handleEOS();
            });
        }

        gst_clear_message(&forward_msg);
        break;
    }
    default:
        break;
    }

    return TRUE;
}

void GstVideoReceiver::_onNewPad(GstElement *element, GstPad *pad, gpointer data)
{
    GstVideoReceiver *self = static_cast<GstVideoReceiver*>(data);

    qWarning() << "GstVideoReceiver::_onNewPad() called for element:" << element;

    if (element == self->_source) {
        qWarning() << "GstVideoReceiver: New source pad detected, calling _onNewSourcePad";
        self->_onNewSourcePad(pad);
    } else if (element == self->_decoder) {
        qWarning() << "GstVideoReceiver: New decoder pad detected, calling _onNewDecoderPad";
        self->_onNewDecoderPad(pad);
    } else {
        qCDebug(GstVideoReceiverLog) << "Unexpected call!";
    }
}

void GstVideoReceiver::_wrapWithGhostPad(GstElement *element, GstPad *pad, gpointer data)
{
    Q_UNUSED(data)

    qWarning() << "GstVideoReceiver::_wrapWithGhostPad() called - creating ghost pad";

    gchar *name = gst_pad_get_name(pad);
    if (!name) {
        qCCritical(GstVideoReceiverLog) << "gst_pad_get_name() failed";
        return;
    }

    GstPad *ghostpad = gst_ghost_pad_new(name, pad);
    if (!ghostpad) {
        qCCritical(GstVideoReceiverLog) << "gst_ghost_pad_new() failed";
        g_clear_pointer(&name, g_free);
        return;
    }

    g_clear_pointer(&name, g_free);

    (void) gst_pad_set_active(ghostpad, TRUE);

    GstElement *bin = GST_ELEMENT_PARENT(element);
    if (!gst_element_add_pad(bin, ghostpad)) {
        qCCritical(GstVideoReceiverLog) << "gst_element_add_pad() failed";
        return;
    }

    qWarning() << "GstVideoReceiver: Ghost pad created successfully";
}

void GstVideoReceiver::_linkPad(GstElement *element, GstPad *pad, gpointer data)
{
    gchar *name = gst_pad_get_name(pad);
    if (!name) {
        qCCritical(GstVideoReceiverLog) << "gst_pad_get_name() failed";
        return;
    }

    if (!gst_element_link_pads(element, name, GST_ELEMENT(data), "sink")) {
        qCCritical(GstVideoReceiverLog) << "gst_element_link_pads() failed";
    }

    g_clear_pointer(&name, g_free);
}

gboolean GstVideoReceiver::_padProbe(GstElement *element, GstPad *pad, gpointer user_data)
{
    Q_UNUSED(element)

    int *probeRes = static_cast<int*>(user_data);
    *probeRes |= 1;

    GstCaps *filter = gst_caps_from_string("application/x-rtp");
    if (filter) {
        GstCaps *caps = gst_pad_query_caps(pad, nullptr);
        if (caps) {
            if (!gst_caps_is_any(caps) && gst_caps_can_intersect(caps, filter)) {
                *probeRes |= 2;
            }

            gst_clear_caps(&caps);
        }

        gst_clear_caps(&filter);
    }

    return TRUE;
}

GstPadProbeReturn GstVideoReceiver::_teeProbe(GstPad *pad, GstPadProbeInfo *info, gpointer user_data)
{
    Q_UNUSED(pad); Q_UNUSED(info)

    if (user_data) {
        GstVideoReceiver *pThis = static_cast<GstVideoReceiver*>(user_data);
        pThis->_noteTeeFrame();
    }

    return GST_PAD_PROBE_OK;
}

GstPadProbeReturn GstVideoReceiver::_videoSinkProbe(GstPad *pad, GstPadProbeInfo *info, gpointer user_data)
{
    Q_UNUSED(pad); Q_UNUSED(info)

    if (user_data) {
        GstVideoReceiver *pThis = static_cast<GstVideoReceiver*>(user_data);

        if (pThis->_resetVideoSink) {
            pThis->_resetVideoSink = false;
        }

        pThis->_noteVideoSinkFrame();
    }

    return GST_PAD_PROBE_OK;
}

GstPadProbeReturn GstVideoReceiver::_eosProbe(GstPad *pad, GstPadProbeInfo *info, gpointer user_data)
{
    Q_UNUSED(pad);
    Q_ASSERT(user_data);

    if (info) {
        const GstEvent *event = gst_pad_probe_info_get_event(info);
        if (GST_EVENT_TYPE(event) == GST_EVENT_EOS) {
            GstVideoReceiver *pThis = static_cast<GstVideoReceiver*>(user_data);
            pThis->_noteEndOfStream();
        }
    }

    return GST_PAD_PROBE_OK;
}

GstPadProbeReturn GstVideoReceiver::_keyframeWatch(GstPad *pad, GstPadProbeInfo *info, gpointer user_data)
{
    if (!info || !user_data) {
        qCCritical(GstVideoReceiverLog) << "Invalid arguments";
        return GST_PAD_PROBE_DROP;
    }

    GstBuffer *buf = gst_pad_probe_info_get_buffer(info);
    if (GST_BUFFER_FLAG_IS_SET(buf, GST_BUFFER_FLAG_DELTA_UNIT)) {
        // wait for a keyframe
        return GST_PAD_PROBE_DROP;
    }

    // set media file '0' offset to current timeline position - we don't want to touch other elements in the graph, except these which are downstream!
    gst_pad_set_offset(pad, -static_cast<gint64>(buf->pts));

    qCDebug(GstVideoReceiverLog) << "Got keyframe, stop dropping buffers";

    GstVideoReceiver *pThis = static_cast<GstVideoReceiver*>(user_data);
    pThis->_dispatchSignal([pThis]() { emit pThis->recordingStarted(pThis->recordingOutput()); });

    return GST_PAD_PROBE_REMOVE;
}

GstVideoWorker::GstVideoWorker(QObject *parent)
    : QThread(parent)
{
    // qCDebug(GstVideoReceiverLog) << this;
}

GstVideoWorker::~GstVideoWorker()
{
    // qCDebug(GstVideoReceiverLog) << this;
}

bool GstVideoWorker::needDispatch() const
{
    return (QThread::currentThread() != this);
}

void GstVideoWorker::dispatch(Task task)
{
    QMutexLocker lock(&_taskQueueSync);
    _taskQueue.enqueue(task);
    _taskQueueUpdate.wakeOne();
}

void GstVideoWorker::shutdown()
{
    if (needDispatch()) {
        dispatch([this]() { _shutdown = true; });
        (void) QThread::wait(2000);
    } else {
        QThread::quit();
    }
}

void GstVideoWorker::run()
{
    while (!_shutdown) {
        _taskQueueSync.lock();

        while (_taskQueue.isEmpty()) {
            _taskQueueUpdate.wait(&_taskQueueSync);
        }

        const Task task = _taskQueue.dequeue();

        _taskQueueSync.unlock();

        task();
    }
}
