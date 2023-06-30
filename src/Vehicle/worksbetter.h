#ifndef WORKSBETTER_H
#define WORKSBETTER_H

#include <queue>
#include "QObject"
#include "QTcpSocket"
#include "QGCLoggingCategory.h"

Q_DECLARE_LOGGING_CATEGORY(WorksbetterLog)

class Worksbetter : public QObject
{
    Q_OBJECT

private:
    QTcpSocket *_socket = new QTcpSocket();
    QString _ipAddress;
    QString _response;
    bool _socketIsConnected;
    bool _responseHandlerIsBound;
    static const char*  _wirisIpAddressKey;

    void _write_command(QString command);
    quint8 _check_success(QString cmp_val);

    std::queue<QString> _commandQueue;

private slots:
    void _response_handler();

signals:
    /// the result of an ethernet command to the camera
    /// @param command: The command with no parameters
    /// @param commandFull: The entire raw command including parameters that was sent
    /// @param response: The response from the command
    void commandResult               (QString command, QString commandFull, QString response);

public:
    Worksbetter();
    ~Worksbetter();

    Q_INVOKABLE void connect();
    Q_INVOKABLE void disconnect();
    Q_INVOKABLE bool isConnected();

    Q_INVOKABLE QString get_ip_address();
    Q_INVOKABLE void set_ip_address(QString address);

    Q_INVOKABLE void write_command(QString key);

    //API Commands
    Q_INVOKABLE quint8 set_activation_key(QString key);
    Q_INVOKABLE quint8 get_activation_state();
    Q_INVOKABLE void set_ethernet_mode(quint8 mode);
    Q_INVOKABLE quint8 get_ethernet_mode();
    Q_INVOKABLE void SDLM();
    Q_INVOKABLE void GDLM();
    Q_INVOKABLE quint8 is_connected();
    Q_INVOKABLE QString get_serial_num();
    Q_INVOKABLE void MOVE_UP();
    Q_INVOKABLE void MOVE_DOWN();
    Q_INVOKABLE void MOVE_OK();
    Q_INVOKABLE void MOVE_CANCEL();
    Q_INVOKABLE void GATN();
    Q_INVOKABLE void GFWV();
    Q_INVOKABLE void GTCU();
    Q_INVOKABLE void GTIC();
    Q_INVOKABLE void GTIN();
    Q_INVOKABLE void GFPW();
    Q_INVOKABLE void MOVE();
    Q_INVOKABLE void GGPS();
    Q_INVOKABLE void GTUT();
    Q_INVOKABLE void STUT();
    Q_INVOKABLE void GRMD();
    Q_INVOKABLE void SRMD();
    Q_INVOKABLE void GRMM();
    Q_INVOKABLE void SRMM();
    Q_INVOKABLE void GRWC();
    Q_INVOKABLE void SRWC();
    Q_INVOKABLE void GREN();
    Q_INVOKABLE void GREL();
    Q_INVOKABLE void SREN();
    Q_INVOKABLE void GTST();
    Q_INVOKABLE void STST();
    Q_INVOKABLE void GHRJ();
    Q_INVOKABLE void SHRJ();
    Q_INVOKABLE void GCRJ();
    Q_INVOKABLE void SCRJ();
    Q_INVOKABLE void GCWM();
    Q_INVOKABLE void SCWM();
    Q_INVOKABLE void GCAT();
    Q_INVOKABLE void SCAT();
    Q_INVOKABLE void GUSL();
    Q_INVOKABLE void SUSL();
    Q_INVOKABLE void GLSL();
    Q_INVOKABLE void SLSL();
    Q_INVOKABLE void GCRP();
    Q_INVOKABLE void SCRP();
    Q_INVOKABLE void GCHY();
    Q_INVOKABLE void SCHY();
    Q_INVOKABLE void GEDF();
    Q_INVOKABLE void GEDS();
    Q_INVOKABLE void GINB();
    Q_INVOKABLE void SINB();
    Q_INVOKABLE void GSLB();
    Q_INVOKABLE void SSLB();
    Q_INVOKABLE void GCWP();
    Q_INVOKABLE void GCGR();
    Q_INVOKABLE void GLEN();
    Q_INVOKABLE void GLEL();
    Q_INVOKABLE void SLEN();
    Q_INVOKABLE void GCWV();
    Q_INVOKABLE void SCWV();
    Q_INVOKABLE void GGPI();
    Q_INVOKABLE void SGPI();
    Q_INVOKABLE void GIIN();
    Q_INVOKABLE void SIIN();
    Q_INVOKABLE void SLAY();
    Q_INVOKABLE void GMCA();
    Q_INVOKABLE void SMCA();
    Q_INVOKABLE void STTY();
    Q_INVOKABLE void SZIN();
    Q_INVOKABLE void SZOT();
    Q_INVOKABLE void GZTV();
    Q_INVOKABLE void GZTL();
    Q_INVOKABLE void SZTN();
    Q_INVOKABLE void GZSM();
    Q_INVOKABLE void SZSM();
    Q_INVOKABLE void GZVV();
    Q_INVOKABLE void GZVL();
    Q_INVOKABLE void SZVN();
    Q_INVOKABLE void SGFA();
    Q_INVOKABLE void SGFI();
    Q_INVOKABLE void GDGM();
    Q_INVOKABLE void SDGM();
    Q_INVOKABLE void GHSM();
    Q_INVOKABLE void SHSM();
    Q_INVOKABLE void GCDT();
    Q_INVOKABLE void GPTE();
    Q_INVOKABLE void GPTL();
    Q_INVOKABLE void SPTE();
    Q_INVOKABLE void SPTI();
    Q_INVOKABLE void GCMP();
    Q_INVOKABLE void GCML();
    Q_INVOKABLE void SCMP();
    Q_INVOKABLE void CPTR();
    Q_INVOKABLE void ICPT();
    Q_INVOKABLE void RCRS();
    Q_INVOKABLE void RCRF();
    Q_INVOKABLE void IRCR();
    Q_INVOKABLE void IMCR();
    Q_INVOKABLE void GCTY();
    Q_INVOKABLE void SCTY();
    Q_INVOKABLE void GPIS();
    Q_INVOKABLE void SPIS();
    Q_INVOKABLE void GPIC();
    Q_INVOKABLE void SPIC();
    Q_INVOKABLE void GGFT();
    Q_INVOKABLE void SGFT();
    Q_INVOKABLE void GBFL();
    Q_INVOKABLE void SBFL();
    Q_INVOKABLE void GEFL();
    Q_INVOKABLE void SEFL();
    Q_INVOKABLE void GCSB();
    Q_INVOKABLE void SCSB();
    Q_INVOKABLE void GALM();
    Q_INVOKABLE void SALM();
    Q_INVOKABLE void GALV();
    Q_INVOKABLE void SALV();
    Q_INVOKABLE void GALC();
    Q_INVOKABLE void SALC();
    Q_INVOKABLE void CUPD();
    Q_INVOKABLE void IUPD();
    Q_INVOKABLE void GTEM();
    Q_INVOKABLE void STEM();
    Q_INVOKABLE void GTRT();
    Q_INVOKABLE void STRT();
    Q_INVOKABLE void GTAT();
    Q_INVOKABLE void STAT();
    Q_INVOKABLE void GTII();
    Q_INVOKABLE void STII();
    Q_INVOKABLE void GTCP();
    Q_INVOKABLE void STCP();
    Q_INVOKABLE void GTSC();
    Q_INVOKABLE void STSC();
    Q_INVOKABLE void DTSR();
    Q_INVOKABLE void GTSN();
    Q_INVOKABLE void GTSL();
    Q_INVOKABLE void GIRJ();
    Q_INVOKABLE void GIRT();
    Q_INVOKABLE void GICT();
    Q_INVOKABLE void GISS();
    Q_INVOKABLE void GISR();
    Q_INVOKABLE void GIVI();
    Q_INVOKABLE void GIHR();
    Q_INVOKABLE void GVTH();
    Q_INVOKABLE void GVTE();
    Q_INVOKABLE void GVVI();
    Q_INVOKABLE void GVHR();
    Q_INVOKABLE void SIRJ();
    Q_INVOKABLE void SIRT();
    Q_INVOKABLE void SICT();
    Q_INVOKABLE void SISS();
    Q_INVOKABLE void SISR();
    Q_INVOKABLE void SIVI();
    Q_INVOKABLE void SIHR();
    Q_INVOKABLE void SVTH();
    Q_INVOKABLE void SVTE();
    Q_INVOKABLE void SVVI();
    Q_INVOKABLE void SVHR();
    Q_INVOKABLE void GILC();
    Q_INVOKABLE void SVLC();
    Q_INVOKABLE void GVLC();
    Q_INVOKABLE void SILC();
    Q_INVOKABLE void GHRR();
    Q_INVOKABLE void SHRR();
    Q_INVOKABLE void GLRF();
    Q_INVOKABLE void SLRF();
    Q_INVOKABLE void GDTI();
    Q_INVOKABLE void SDTI();
    Q_INVOKABLE void STUS();
    Q_INVOKABLE void GEST();
    Q_INVOKABLE void GMST();
    Q_INVOKABLE void GMSI();
    Q_INVOKABLE void GMFR();
    Q_INVOKABLE void GMCP();
    Q_INVOKABLE void GTRC();
    Q_INVOKABLE void GVRC();
    Q_INVOKABLE void CPST();
    Q_INVOKABLE void CPSS();
    Q_INVOKABLE void GTRE();
    Q_INVOKABLE void GTEX();
    Q_INVOKABLE void GOPT();
    Q_INVOKABLE void GROT();
    Q_INVOKABLE void AREA();
    Q_INVOKABLE void GPLV();
    Q_INVOKABLE void GTRG();
    Q_INVOKABLE void STRG();
    Q_INVOKABLE void SDST();
    Q_INVOKABLE void SHDW();
    Q_INVOKABLE void REBT();
    Q_INVOKABLE void SSSS();
    Q_INVOKABLE void GSSS();
    Q_INVOKABLE void SSGS();
    Q_INVOKABLE void GSGS();
    Q_INVOKABLE void SSIR();
    Q_INVOKABLE void GSIR();
    Q_INVOKABLE void STSB();
    Q_INVOKABLE void GTSB();
    Q_INVOKABLE void SVSB();
    Q_INVOKABLE void GVSB();
};

#endif // WORKSBETTER_H
