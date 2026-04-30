#pragma once

#include <QObject>
#include <QTimer>

#include "MAVLinkProtocol.h"
#include "VideoSettings.h"
#include "SettingsManager.h"

Q_DECLARE_LOGGING_CATEGORY(VideoStreamControlLog)

class VideoStreamControl : public QObject
{
    Q_OBJECT
public:
    VideoStreamControl();
    ~VideoStreamControl();

    Q_PROPERTY(bool settingInProgress READ settingInProgress NOTIFY settingInProgressChanged)
    bool settingInProgress() { return _settingInProgress; }

    Q_PROPERTY(uint8_t currentHdmiInput READ currentHdmiInput NOTIFY currentHdmiInputChanged)
    uint8_t currentHdmiInput() { return _currentHdmiInput; }

    Q_PROPERTY(bool cameraInfoReceived READ cameraInfoReceived NOTIFY cameraInfoReceivedChanged)
    bool cameraInfoReceived() { return _cameraInfoReceived; }

    Q_PROPERTY(uint8_t currentResolution READ currentResolution NOTIFY currentResolutionChanged)
    uint8_t currentResolution() { return _currentResolution; }

    Q_PROPERTY(uint8_t currentActiveSource READ currentActiveSource NOTIFY currentActiveSourceChanged)
    uint8_t currentActiveSource() { return _currentActiveSource; }

signals:
    void settingInProgressChanged();
    void videoNeedsReset();
    void decodingNeedsRestart();  // Lighter-weight reset: just restart decoder/sink, keep RTSP alive
    void currentHdmiInputChanged();
    void cameraInfoReceivedChanged();
    void currentResolutionChanged();
    void currentActiveSourceChanged();

private slots:
    void _mavlinkMessageReceived(LinkInterface *link, mavlink_message_t message);
    void _cameraIdChanged();
    void _activeSourceChanged();
    void _resolutionChanged();
    void _requestVideoStreamInfo();

private:
    int _systemId;
    LinkInterface *_linkInterface;
    MAVLinkProtocol *_mavlinkProtocol;
    VideoSettings *_videoSettings;
    QTimer _infoRequestTimer;
    uint32_t _cameraServiceUid;
    uint32_t _cameraCount;
    uint32_t _cameraIdSetting;
    bool _settingInProgress;
    bool _cameraInfoReceived;
    bool _resolutionSwitchPending;  // Suppress encoder resets during resolution change
    uint8_t _currentHdmiInput;
    uint8_t _currentResolution;
    uint8_t _currentActiveSource;  // 0=HDMI1, 1=HDMI2, 2=RTSP1, 3=RTSP2

    void _handleHeartbeatInfo(LinkInterface* link, mavlink_message_t& message);
    void _handleVideoStreamInformation(mavlink_message_t& message);
    void _setCameraIdLockUi(bool lockUi);
    void _startVideoStreaming();
    void _setSettingInProgress(bool inProgress);
};
