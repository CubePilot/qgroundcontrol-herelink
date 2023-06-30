/*****************************************
* Author: Joseph Albanese
* Date: 05/2023
*****************************************/

#include <queue>
#include "worksbetter.h"
#include "qeventloop.h"
#include "QGCApplication.h"
#include "IGCSettings.h"
#include "SettingsManager.h"

QGC_LOGGING_CATEGORY(WorksbetterLog,     "WorksbetterLog")

#define WIRIS_PORT 2240

// private
void Worksbetter::_write_command(QString command) {
    if (_socketIsConnected) {
        _commandQueue.push(command);

        qCWarning(WorksbetterLog) << "** ETH >> curReq: " << command;
        _socket->write(command.toStdString().c_str(), command.length());
        _socket->flush();        
    } else {
        qCWarning(WorksbetterLog()) << "There is no connection to a camera. Attempt reconnect.";
        connect();
    }
}

quint8 Worksbetter::_check_success(QString cmpVal) {
    QEventLoop loop;
    QAbstractSocket::connect(_socket, &QTcpSocket::readyRead, &loop, &QEventLoop::quit);
    loop.exec();

    if (_response.contains(cmpVal)) {
        return 1;
    }
    return 0;
}

void Worksbetter::_response_handler() {
    _response = QString(_socket->readAll()).trimmed();

    QString commandFull = _commandQueue.front();
    _commandQueue.pop();

    if (commandFull.endsWith('\n')) {
        commandFull.chop(1);
    }

    QStringList words = commandFull.split(' ', QString::SkipEmptyParts);
    QString command = words.value(0).trimmed();

    qCDebug(WorksbetterLog) << "processed: " << commandFull << " -> " << _response;
    emit commandResult(command, commandFull, _response);
}

//public
Worksbetter::Worksbetter() {
    _socketIsConnected = false;
    _responseHandlerIsBound = false;

    // attempt to connect
    IGCSettings* igcSettings = qgcApp()->toolbox()->settingsManager()->igcSettings();
    bool isEthernetCommEnabled = igcSettings->isEthernetCommEnabled()->rawValue().toBool();

    if (isEthernetCommEnabled) {
        connect();
    }
}

Worksbetter::~Worksbetter() {
    QAbstractSocket::disconnect(_socket, SIGNAL(readyRead()), this, SLOT(_response_handler()));
    delete _socket;
}

void Worksbetter::connect() {
    _ipAddress = get_ip_address();

    if(_socket) {
        if(_socket->state() == QAbstractSocket::ConnectedState) {
            _socketIsConnected = true;
            qCWarning(WorksbetterLog) << "Camera is connected.";
            return;
        } else {
            _socketIsConnected = false;
            qCWarning(WorksbetterLog) << "State: " << _socket->state();
        }
    } else {
        _socketIsConnected = false;
        qCWarning(WorksbetterLog) << "Fatal: No socket found.";
        return;
    }

    qCWarning(WorksbetterLog) << "attempt to connect to camera at " << _ipAddress;
    _socket->connectToHost(_ipAddress, WIRIS_PORT);

    if (!_responseHandlerIsBound) {
        bool connection = QAbstractSocket::connect(_socket, SIGNAL(readyRead()), this, SLOT(_response_handler()));
        if (!connection) {
            qCWarning(WorksbetterLog) << "Unable to connect to a camera. Confirm the IP Address is correct in IGC settings.";
            return;
        }
        _responseHandlerIsBound = true;
    }
    _socketIsConnected = true;
    _commandQueue = std::queue<QString>();
    qCDebug(WorksbetterLog) << "Camera connected";
}

void Worksbetter::disconnect() {
    _socket->close();
}

bool Worksbetter::isConnected() {
    return _socketIsConnected;
}

// don't use this
void Worksbetter::set_ip_address(QString address) {
    _ipAddress = address;
}

QString Worksbetter::get_ip_address() {
    IGCSettings* igcSettings = qgcApp()->toolbox()->settingsManager()->igcSettings();
    QString ipAddress = igcSettings->cameraIpAddress()->rawValue().toString();

    return ipAddress;
}

void Worksbetter::write_command(QString command) {
    _write_command(command);
}

// Returns 1, ERR if activation key length is incorrect
quint8 Worksbetter::set_activation_key(QString key) {
    static const unsigned char keyLen = 14;

    if (key.length() == keyLen){
        _write_command("ACTV " + key + "\n");
        return 0;
    }
    return 1;
}

// Retruns 1 if camera Ethernet SDK is activated
quint8 Worksbetter::get_activation_state() {
    _write_command("IACT\n");
    return _check_success("TRUE");
}

// mode = 1 -> ethernet activated
// mode = 0 -> ethernet deactivated
void Worksbetter::set_ethernet_mode(quint8 mode) {
    if (mode) {
        _write_command("SETH TRUE\n");
    } else {
        _write_command("SETH FALSE\n");
    }
}

// mode = 1 -> ethernet activated
// mode = 0 -> ethernet deactivated
quint8 Worksbetter::get_ethernet_mode() {
    _write_command("GETH\n");
    return _check_success("TRUE");
}
void Worksbetter::SDLM() {
    _write_command("SDLM\n");
}
void Worksbetter::GDLM() {
    _write_command("GDLM\n");
}
quint8 Worksbetter::is_connected() {
    _write_command("HIWS\n");
    return _check_success("OK");
}
QString Worksbetter::get_serial_num() {
    _write_command("GSRN\n");
    return _socket->read(14);
}
void Worksbetter::MOVE_UP() {
    _write_command("MOVE UP\n");
}
void Worksbetter::MOVE_DOWN() {
    _write_command("MOVE DOWN\n");
}
void Worksbetter::MOVE_OK() {
    _write_command("MOVE OK\n");
}
void Worksbetter::MOVE_CANCEL() {
    _write_command("MOVE CANCEL\n");
}
void Worksbetter::GATN() {
    _write_command("GATN\n");
}
void Worksbetter::GFWV() {
    _write_command("GFWV\n");
}
void Worksbetter::GTCU() {
    _write_command("GTCU\n");
}
void Worksbetter::GTIC() {
    _write_command("GTIC\n");
}
void Worksbetter::GTIN() {
    _write_command("GTIN\n");
}
void Worksbetter::GFPW() {
    _write_command("GFPW\n");
}
void Worksbetter::MOVE() {
    _write_command("MOVE\n");
}
void Worksbetter::GGPS() {
    _write_command("GGPS\n");
}
void Worksbetter::GTUT() {
    _write_command("GTUT\n");
}
void Worksbetter::STUT() {
    _write_command("STUT\n");
}
void Worksbetter::GRMD() {
    _write_command("GRMD\n");
}
void Worksbetter::SRMD() {
    _write_command("SRMD\n");
}
void Worksbetter::GRMM() {
    _write_command("GRMM\n");
}
void Worksbetter::SRMM() {
    _write_command("SRMM\n");
}
void Worksbetter::GRWC() {
    _write_command("GRWC\n");
}
void Worksbetter::SRWC() {
    _write_command("SRWC\n");
}
void Worksbetter::GREN() {
    _write_command("GREN\n");
}
void Worksbetter::GREL() {
    _write_command("GREL\n");
}
void Worksbetter::SREN() {
    _write_command("SREN\n");
}
void Worksbetter::GTST() {
    _write_command("GTST\n");
}
void Worksbetter::STST() {
    _write_command("STST\n");
}
void Worksbetter::GHRJ() {
    _write_command("GHRJ\n");
}
void Worksbetter::SHRJ() {
    _write_command("SHRJ\n");
}
void Worksbetter::GCRJ() {
    _write_command("GCRJ\n");
}
void Worksbetter::SCRJ() {
    _write_command("SCRJ\n");
}
void Worksbetter::GCWM() {
    _write_command("GCWM\n");
}
void Worksbetter::SCWM() {
    _write_command("SCWM\n");
}
void Worksbetter::GCAT() {
    _write_command("GCAT\n");
}
void Worksbetter::SCAT() {
    _write_command("SCAT\n");
}
void Worksbetter::GUSL() {
    _write_command("GUSL\n");
}
void Worksbetter::SUSL() {
    _write_command("SUSL\n");
}
void Worksbetter::GLSL() {
    _write_command("GLSL\n");
}
void Worksbetter::SLSL() {
    _write_command("SLSL\n");
}
void Worksbetter::GCRP() {
    _write_command("GCRP\n");
}
void Worksbetter::SCRP() {
    _write_command("SCRP\n");
}
void Worksbetter::GCHY() {
    _write_command("GCHY\n");
}
void Worksbetter::SCHY() {
    _write_command("SCHY\n");
}
void Worksbetter::GEDF() {
    _write_command("GEDF\n");
}
void Worksbetter::GEDS() {
    _write_command("GEDS\n");
}
void Worksbetter::GINB() {
    _write_command("GINB\n");
}
void Worksbetter::SINB() {
    _write_command("SINB\n");
}
void Worksbetter::GSLB() {
    _write_command("GSLB\n");
}
void Worksbetter::SSLB() {
    _write_command("SSLB\n");
}
void Worksbetter::GCWP() {
    _write_command("GCWP\n");
}
void Worksbetter::GCGR() {
    _write_command("GCGR\n");
}
void Worksbetter::GLEN() {
    _write_command("GLEN\n");
}
void Worksbetter::GLEL() {
    _write_command("GLEL\n");
}
void Worksbetter::SLEN() {
    _write_command("SLEN\n");
}
void Worksbetter::GCWV() {
    _write_command("GCWV\n");
}
void Worksbetter::SCWV() {
    _write_command("SCWV\n");
}
void Worksbetter::GGPI() {
    _write_command("GGPI\n");
}
void Worksbetter::SGPI() {
    _write_command("SGPI\n");
}
void Worksbetter::GIIN() {
    _write_command("GIIN\n");
}
void Worksbetter::SIIN() {
    _write_command("SIIN\n");
}
void Worksbetter::SLAY() {
    _write_command("SLAY\n");
}
void Worksbetter::GMCA() {
    _write_command("GMCA\n");
}
void Worksbetter::SMCA() {
    _write_command("SMCA\n");
}
void Worksbetter::STTY() {
    _write_command("STTY\n");
}
void Worksbetter::SZIN() {
    _write_command("SZIN\n");
}
void Worksbetter::SZOT() {
    _write_command("SZOT\n");
}
void Worksbetter::GZTV() {
    _write_command("GZTV\n");
}
void Worksbetter::GZTL() {
    _write_command("GZTL\n");
}
void Worksbetter::SZTN() {
    _write_command("SZTN\n");
}
void Worksbetter::GZSM() {
    _write_command("GZSM\n");
}
void Worksbetter::SZSM() {
    _write_command("SZSM\n");
}
void Worksbetter::GZVV() {
    _write_command("GZVV\n");
}
void Worksbetter::GZVL() {
    _write_command("GZVL\n");
}
void Worksbetter::SZVN() {
    _write_command("SZVN\n");
}
void Worksbetter::SGFA() {
    _write_command("SGFA\n");
}
void Worksbetter::SGFI() {
    _write_command("SGFI\n");
}
void Worksbetter::GDGM() {
    _write_command("GDGM\n");
}
void Worksbetter::SDGM() {
    _write_command("SDGM\n");
}
void Worksbetter::GHSM() {
    _write_command("GHSM\n");
}
void Worksbetter::SHSM() {
    _write_command("SHSM\n");
}
void Worksbetter::GCDT() {
    _write_command("GCDT\n");
}
void Worksbetter::GPTE() {
    _write_command("GPTE\n");
}
void Worksbetter::GPTL() {
    _write_command("GPTL\n");
}
void Worksbetter::SPTE() {
    _write_command("SPTE\n");
}
void Worksbetter::SPTI() {
    _write_command("SPTI 8\n");
}
void Worksbetter::GCMP() {
    _write_command("GCMP\n");
}
void Worksbetter::GCML() {
    _write_command("GCML\n");
}
void Worksbetter::SCMP() {
    _write_command("SCMP\n");
}
void Worksbetter::CPTR() {
    _write_command("CPTR\n");
}
void Worksbetter::ICPT() {
    _write_command("ICPT\n");
}
void Worksbetter::RCRS() {
    _write_command("RCRS\n");
}
void Worksbetter::RCRF() {
    _write_command("RCRF\n");
}
void Worksbetter::IRCR() {
    _write_command("IRCR\n");
}
void Worksbetter::IMCR() {
    _write_command("IMCR\n");
}
void Worksbetter::GCTY() {
    _write_command("GCTY\n");
}
void Worksbetter::SCTY() {
    _write_command("SCTY\n");
}
void Worksbetter::GPIS() {
    _write_command("GPIS\n");
}
void Worksbetter::SPIS() {
    _write_command("SPIS\n");
}
void Worksbetter::GPIC() {
    _write_command("GPIC\n");
}
void Worksbetter::SPIC() {
    _write_command("SPIC\n");
}
void Worksbetter::GGFT() {
    _write_command("GGFT\n");
}
void Worksbetter::SGFT() {
    _write_command("SGFT\n");
}
void Worksbetter::GBFL() {
    _write_command("GBFL\n");
}
void Worksbetter::SBFL() {
    _write_command("SBFL\n");
}
void Worksbetter::GEFL() {
    _write_command("GEFL\n");
}
void Worksbetter::SEFL() {
    _write_command("SEFL\n");
}
void Worksbetter::GCSB() {
    _write_command("GCSB\n");
}
void Worksbetter::SCSB() {
    _write_command("SCSB\n");
}
void Worksbetter::GALM() {
    _write_command("GALM\n");
}
void Worksbetter::SALM() {
    _write_command("SALM\n");
}
void Worksbetter::GALV() {
    _write_command("GALV\n");
}
void Worksbetter::SALV() {
    _write_command("SALV\n");
}
void Worksbetter::GALC() {
    _write_command("GALC\n");
}
void Worksbetter::SALC() {
    _write_command("SALC\n");
}
void Worksbetter::CUPD() {
    _write_command("CUPD\n");
}
void Worksbetter::IUPD() {
    _write_command("IUPD\n");
}
void Worksbetter::GTEM() {
    _write_command("GTEM\n");
}
void Worksbetter::STEM() {
    _write_command("STEM\n");
}
void Worksbetter::GTRT() {
    _write_command("GTRT\n");
}
void Worksbetter::STRT() {
    _write_command("STRT\n");
}
void Worksbetter::GTAT() {
    _write_command("GTAT\n");
}
void Worksbetter::STAT() {
    _write_command("STAT\n");
}
void Worksbetter::GTII() {
    _write_command("GTII\n");
}
void Worksbetter::STII() {
    _write_command("STII\n");
}
void Worksbetter::GTCP() {
    _write_command("GTCP\n");
}
void Worksbetter::STCP() {
    _write_command("STCP\n");
}
void Worksbetter::GTSC() {
    _write_command("GTSC\n");
}
void Worksbetter::STSC() {
    _write_command("STSC\n");
}
void Worksbetter::DTSR() {
    _write_command("DTSR\n");
}
void Worksbetter::GTSN() {
    _write_command("GTSN\n");
}
void Worksbetter::GTSL() {
    _write_command("GTSL\n");
}
void Worksbetter::GIRJ() {
    _write_command("GIRJ\n");
}
void Worksbetter::GIRT() {
    _write_command("GIRT\n");
}
void Worksbetter::GICT() {
    _write_command("GICT\n");
}
void Worksbetter::GISS() {
    _write_command("GISS\n");
}
void Worksbetter::GISR() {
    _write_command("GISR\n");
}
void Worksbetter::GIVI() {
    _write_command("GIVI\n");
}
void Worksbetter::GIHR() {
    _write_command("GIHR\n");
}
void Worksbetter::GVTH() {
    _write_command("GVTH\n");
}
void Worksbetter::GVTE() {
    _write_command("GVTE\n");
}
void Worksbetter::GVVI() {
    _write_command("GVVI\n");
}
void Worksbetter::GVHR() {
    _write_command("GVHR\n");
}
void Worksbetter::SIRJ() {
    _write_command("SIRJ\n");
}
void Worksbetter::SIRT() {
    _write_command("SIRT\n");
}
void Worksbetter::SICT() {
    _write_command("SICT\n");
}
void Worksbetter::SISS() {
    _write_command("SISS\n");
}
void Worksbetter::SISR() {
    _write_command("SISR\n");
}
void Worksbetter::SIVI() {
    _write_command("SIVI\n");
}
void Worksbetter::SIHR() {
    _write_command("SIHR\n");
}
void Worksbetter::SVTH() {
    _write_command("SVTH\n");
}
void Worksbetter::SVTE() {
    _write_command("SVTE\n");
}
void Worksbetter::SVVI() {
    _write_command("SVVI\n");
}
void Worksbetter::SVHR() {
    _write_command("SVHR\n");
}
void Worksbetter::GILC() {
    _write_command("GILC\n");
}
void Worksbetter::SVLC() {
    _write_command("SVLC\n");
}
void Worksbetter::GVLC() {
    _write_command("GVLC\n");
}
void Worksbetter::SILC() {
    _write_command("SILC\n");
}
void Worksbetter::GHRR() {
    _write_command("GHRR\n");
}
void Worksbetter::SHRR() {
    _write_command("SHRR\n");
}
void Worksbetter::GLRF() {
    _write_command("GLRF\n");
}
void Worksbetter::SLRF() {
    _write_command("SLRF\n");
}
void Worksbetter::GDTI() {
    _write_command("GDTI\n");
}
void Worksbetter::SDTI() {
    _write_command("SDTI\n");
}
void Worksbetter::STUS() {
    _write_command("STUS\n");
}
void Worksbetter::GEST() {
    _write_command("GEST\n");
}
void Worksbetter::GMST() {
    _write_command("GMST\n");
}
void Worksbetter::GMSI() {
    _write_command("GMSI\n");
}
void Worksbetter::GMFR() {
    _write_command("GMFR\n");
}
void Worksbetter::GMCP() {
    _write_command("GMCP\n");
}
void Worksbetter::GTRC() {
    _write_command("GTRC\n");
}
void Worksbetter::GVRC() {
    _write_command("GVRC\n");
}
void Worksbetter::CPST() {
    _write_command("CPST\n");
}
void Worksbetter::CPSS() {
    _write_command("CPSS\n");
}
void Worksbetter::GTRE() {
    _write_command("GTRE\n");
}
void Worksbetter::GTEX() {
    _write_command("GTEX\n");
}
void Worksbetter::GOPT() {
    _write_command("GOPT\n");
}
void Worksbetter::GROT() {
    _write_command("GROT\n");
}
void Worksbetter::AREA() {
    _write_command("AREA\n");
}
void Worksbetter::GPLV() {
    _write_command("GPLV\n");
}
void Worksbetter::GTRG() {
    _write_command("GTRG\n");
}
void Worksbetter::STRG() {
    _write_command("STRG\n");
}
void Worksbetter::SDST() {
    _write_command("SDST\n");
}
void Worksbetter::SHDW() {
    _write_command("SHDW\n");
}
void Worksbetter::REBT() {
    _write_command("REBT\n");
}
void Worksbetter::SSSS() {
    _write_command("SSSS\n");
}
void Worksbetter::GSSS() {
    _write_command("GSSS\n");
}
void Worksbetter::SSGS() {
    _write_command("SSGS\n");
}
void Worksbetter::GSGS() {
    _write_command("GSGS\n");
}
void Worksbetter::SSIR() {
    _write_command("SSIR\n");
}
void Worksbetter::GSIR() {
    _write_command("GSIR\n");
}
void Worksbetter::STSB() {
    _write_command("STSB\n");
}
void Worksbetter::GTSB() {
    _write_command("GTSB\n");
}
void Worksbetter::SVSB() {
    _write_command("SVSB\n");
}
void Worksbetter::GVSB() {
    _write_command("GVSB\n");
}
