#pragma once

#include <QObject>
#include <QString>
#include <QMetaObject>
#include <QByteArray>

#include "QGCMAVLink.h"
#include "Vehicle.h"

class Vehicle;

class AscentCC : public QObject
{
    Q_OBJECT

public:
    AscentCC(void);
    Q_PROPERTY(int cameraYaw READ cameraYaw NOTIFY cameraYawChanged)
    Q_PROPERTY(int cameraType READ cameraType NOTIFY cameraTypeChanged)
    Q_INVOKABLE void sendSpeeds(int pitch, int yaw){_sendSpeeds(pitch, yaw);}
    Q_INVOKABLE void zoomSpeed(int speed){_zoomSpeed(speed);}
    Q_INVOKABLE void updateZoomLevel(){_updateZoomLevel();}
    Q_INVOKABLE void centerOnPixel(int x, int y){_centerOnPixel(x, y);}
    Q_INVOKABLE void centerCam(){_centerCam();}
    Q_INVOKABLE void takePicture(){_takePicture();}
    Q_INVOKABLE void record(){_record();}
    Q_INVOKABLE void stopRecording(){_stopRecording();}    
    Q_INVOKABLE void eo(){_eo();}    
    Q_INVOKABLE void ir(){_ir();}    
    Q_INVOKABLE void eoir(){_eoir();}    
    Q_INVOKABLE void ireo(){_ireo();}    
    Q_INVOKABLE void nextPallette(){_nextPallette();}
    Q_INVOKABLE void toggleHeat(){_toggleHeat();}                 
    Q_INVOKABLE void digiZoom1(){_digiZoom1();}    
    Q_INVOKABLE void digiZoom2(){_digiZoom2();}    
    Q_INVOKABLE void digiZoom4(){_digiZoom4();}
    Q_INVOKABLE void digiZoom8(){_digiZoom8();}
    Q_INVOKABLE void digiZoom16(){_digiZoom16();}
    Q_INVOKABLE void trackPoint(int x, int y){_trackPoint(x, y);} 
    Q_INVOKABLE void stopTracking(){_stopTracking();}
    Q_INVOKABLE void lookDown(){_lookDown();}
    Q_INVOKABLE void nuc(){_nuc();}
    Q_INVOKABLE void debug(){_debug();}
    Q_INVOKABLE void requestParam(QString id){_requestParam(id);}

    int cameraYaw(void) const {return (int)_yaw; }
    int cameraType(void) const {return (int)_cam_type;}
    bool isPiP(void) const {return (bool)_isPiP;}
    void setIsPiP(bool pip);
    bool eoIsMain(void) const {return (bool)_eoIsMain;}
    void setEOIsMain(bool eo);

signals:
    void cameraYawChanged(int cameraYaw);
    void cameraTypeChanged(int cameraType);
    void isPiPChanged(bool pip);
    void eoIsMainChanged(bool eo);

private slots:
    void _setActiveVehicle                  (Vehicle* vehicle);
    virtual void _mavlinkMessageReceived    (const mavlink_message_t& message);


protected:
    virtual void   _handleCameraSettings    (const mavlink_message_t& message);
    virtual void   _handleMountOrientation  (const mavlink_message_t& message);
    virtual void   _handleParamValue        (const mavlink_message_t& message);
    MAVLinkProtocol*    _mavlink;
    
private:

    void _sendGimbalSpeeds(int pitch, int yaw);
    void _sendSpeeds(int pitch, int yaw);
    void _zoomSpeed(int speed);
    void _updateZoomLevel();
    void _centerOnPixel(int x, int y);

    void _centerCam();
    void _takePicture();
    void _record();
    void _stopRecording();
    void _ir();
    void _eo();
    void _eoir();
    void _ireo();
    void _nextPallette();
    void _toggleHeat();
    void _digiZoom1();
    void _digiZoom2();
    void _digiZoom4();
    void _digiZoom8();
    void _digiZoom16();
    void _trackPoint(int x, int y);
    void _stopTracking();
    void _lookDown();
    void _nuc();
    void _debug();
    void _requestParam(QString paramid);

    bool        _speedConfig = false;
    int         _compID;
    Vehicle*    _vehicle;
    float       _zoomLevel = 1.0;
    float       _pitch = 0.0;
    float       _yaw = 0.0;
    int         _cam_type = 0;
    bool        _waiting_for_parameter = false;
    bool        _isPiP = false;
    bool        _eoIsMain = true;
};
