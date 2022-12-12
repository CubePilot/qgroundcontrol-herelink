#include "AscentCC.h"
#include "QGCApplication.h"
#include "QGCCorePlugin.h"
#include "UAS.h"
#include "SettingsManager.h"

AscentCC::AscentCC(void)
    :   _compID{100},
        _vehicle{nullptr}
{
    qWarning() << "Setting active vehicle";
    auto *manager = qgcApp()->toolbox()->multiVehicleManager();
    _mavlink = qgcApp()->toolbox()->mavlinkProtocol();
    connect(manager, &MultiVehicleManager::activeVehicleChanged, this, &AscentCC::_setActiveVehicle);
    _setActiveVehicle(manager->activeVehicle());
}

void AscentCC::_setActiveVehicle(Vehicle* vehicle)
{
    _vehicle = vehicle;
    connect(_vehicle, &Vehicle::mavlinkMessageReceived, this, &AscentCC::_mavlinkMessageReceived);
}

void AscentCC::_mavlinkMessageReceived(const mavlink_message_t& message){
    if(message.sysid == _vehicle->id()){
        switch (message.msgid) {
            case MAVLINK_MSG_ID_CAMERA_SETTINGS:
                _handleCameraSettings(message);
                break;
            case MAVLINK_MSG_ID_MOUNT_ORIENTATION:
                _handleMountOrientation(message);
                break;
            case MAVLINK_MSG_ID_PARAM_VALUE:
                if(_waiting_for_parameter){
                    _handleParamValue(message);
                }
                break;
        }
    }
}

void AscentCC::_handleCameraSettings(const mavlink_message_t &message)
{
    mavlink_camera_settings_t cs;
    mavlink_msg_camera_settings_decode(&message, &cs);
    _zoomLevel = cs.zoomLevel;
}

void AscentCC::_handleMountOrientation(const mavlink_message_t &message)
{
    mavlink_mount_orientation_t mo;
    mavlink_msg_mount_orientation_decode(&message, &mo);
    _pitch = mo.pitch;
    _yaw = mo.yaw;
    emit cameraYawChanged(_yaw);
}

void AscentCC::_handleParamValue(const mavlink_message_t &message) 
{ 
    mavlink_param_value_t pv;
    mavlink_msg_param_value_decode(&message, &pv);
    mavlink_param_union_t paramUnion;
    paramUnion.param_float = pv.param_value;
    paramUnion.type = pv.param_type;
    uint16_t val = paramUnion.param_uint16;
    _cam_type = val;
    emit cameraTypeChanged(_cam_type);
    _waiting_for_parameter = false;
}

void AscentCC::_requestParam(QString paramid){
    if(_vehicle) {
        QByteArray ba = paramid.toLocal8Bit();
        const char *id = ba.data();
        mavlink_message_t message;
        auto priority_link = _vehicle->priorityLink();
        mavlink_msg_param_request_read_pack_chan(_mavlink->getSystemId(), _mavlink->getComponentId(), priority_link->mavlinkChannel(), 
                                                &message, _vehicle->id(), _vehicle->defaultComponentId(), id, -1);
        _vehicle->sendMessageOnLink(priority_link, message);
        _waiting_for_parameter = true;
    }
    else{
        qWarning() << "_requestParam: No Active Vehicle";
    }
}

void AscentCC::_sendGimbalSpeeds(int pitch, int yaw){
    auto priority_link = _vehicle->priorityLink();
    auto protocol = qgcApp()->toolbox()->mavlinkProtocol();
    mavlink_message_t msg;
    _vehicle->sendMessageOnLink(priority_link, msg);
}

void AscentCC::_sendSpeeds(int pitch, int yaw){
    if(_vehicle) {
        if(!_speedConfig){
            _speedConfig = true;
            auto priority_link = _vehicle->priorityLink();
            mavlink_message_t message;
            mavlink_msg_command_long_pack_chan(1,
                                            0,
                                            priority_link->mavlinkChannel(),
                                            &message,
                                            1,
                                            100,
                                            MAV_CMD_DO_MOUNT_CONFIGURE,
                                            0,
                                            MAV_MOUNT_MODE_MAVLINK_TARGETING,
                                            1,
                                            1,
                                            1,
                                            1,
                                            1,
                                            1);
            _vehicle->sendMessageOnLink(priority_link, message);
        }
        auto priority_link = _vehicle->priorityLink();
        mavlink_message_t message;
        mavlink_msg_command_long_pack_chan(1,
                                           0,
                                           priority_link->mavlinkChannel(),
                                           &message,
                                           1,
                                           100,
                                           MAV_CMD_DO_MOUNT_CONTROL,
                                           0,
                                           static_cast<float>(pitch) / _zoomLevel,
                                           0,
                                           static_cast<float>(yaw) / _zoomLevel,
                                           0,
                                           0,
                                           0,
                                           MAV_MOUNT_MODE_MAVLINK_TARGETING);
        _vehicle->sendMessageOnLink(priority_link, message);
    }
    else{
        qWarning() << "_sendSpeeds: No Active Vehicle";
    }
}

void AscentCC::_zoomSpeed(int speed){
    if(_vehicle) {
        auto priority_link = _vehicle->priorityLink();
        mavlink_message_t message;
        mavlink_msg_command_long_pack_chan(1,
                                           0,
                                           priority_link->mavlinkChannel(),
                                           &message,
                                           1,
                                           100,
                                           MAV_CMD_SET_CAMERA_ZOOM,
                                           0,
                                           ZOOM_TYPE_CONTINUOUS,
                                           static_cast<float>(speed),
                                           0,
                                           0,
                                           0,
                                           0,
                                           0);
        _vehicle->sendMessageOnLink(priority_link, message);
    }
    else{
        qWarning() << "_zoomInSpeed: No Active Vehicle";
    }
}

void AscentCC::_updateZoomLevel(){
    if(_vehicle) {
    auto priority_link = _vehicle->priorityLink();
    mavlink_message_t message;
    mavlink_msg_command_long_pack_chan(1,
                                    0,
                                    priority_link->mavlinkChannel(),
                                    &message,
                                    1,
                                    100,
                                    MAV_CMD_REQUEST_CAMERA_SETTINGS,
                                    0,
                                    1,
                                    0,
                                    0,
                                    0,
                                    0,
                                    0,
                                    0);
    _vehicle->sendMessageOnLink(priority_link, message);
    }
}

void AscentCC::_centerOnPixel(int x, int y){
    if(_vehicle){
        _speedConfig = false;
        auto priority_link = _vehicle->priorityLink();
        mavlink_message_t message;
        mavlink_msg_command_long_pack_chan(1,
                                            0,
                                            priority_link->mavlinkChannel(),
                                            &message,
                                            1,
                                            100,
                                            MAV_CMD_DO_MOUNT_CONFIGURE,
                                            0,
                                            MAV_MOUNT_MODE_MAVLINK_TARGETING,
                                            1,
                                            1,
                                            1,
                                            0,
                                            0,
                                            0);
        _vehicle->sendMessageOnLink(priority_link, message);
        mavlink_msg_command_long_pack_chan(1,
                                            0,
                                            priority_link->mavlinkChannel(),
                                            &message,
                                            1,
                                            100,
                                            MAV_CMD_DO_MOUNT_CONTROL,
                                            0,
                                            _pitch + (540-y)*0.0368519,
                                            0,
                                            _yaw + (x-960)*0.0277083,
                                            0,
                                            0,
                                            0,
                                            MAV_MOUNT_MODE_MAVLINK_TARGETING);
        _vehicle->sendMessageOnLink(priority_link, message);
    }
    else{
        qWarning() << "_sendSpeeds: No Active Vehicle";
    }
}

void AscentCC::_centerCam(){
    if(_vehicle) {
        auto priority_link = _vehicle->priorityLink();
        mavlink_message_t message;
        mavlink_msg_command_long_pack_chan(1, 0, priority_link->mavlinkChannel(), &message, 1, _vehicle->defaultComponentId(), MAV_CMD_ASCENT_PAYLOAD, 0,
                                           17,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN);
        _vehicle->sendMessageOnLink(priority_link, message);
    }
    else{
        qWarning() << "_centerCam: No Active Vehicle";
    }
}

void AscentCC::_takePicture() {
    if(_vehicle) {
        auto priority_link = _vehicle->priorityLink();
        mavlink_message_t message;
        mavlink_msg_command_long_pack_chan(1, 0, priority_link->mavlinkChannel(), &message, 1, _vehicle->defaultComponentId(), MAV_CMD_ASCENT_PAYLOAD, 0,
                                           0,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN);
        _vehicle->sendMessageOnLink(priority_link, message);
    }
    else{
        qWarning() << "_takePicture: No Active Vehicle";
    }
}

void AscentCC::_record() {
    if(_vehicle) {
        auto priority_link = _vehicle->priorityLink();
        mavlink_message_t message;
        mavlink_msg_command_long_pack_chan(1, 0, priority_link->mavlinkChannel(), &message, 1, _vehicle->defaultComponentId(), MAV_CMD_ASCENT_PAYLOAD, 0,
                                           1,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN);
        _vehicle->sendMessageOnLink(priority_link, message);
    }
    else{
        qWarning() << "_record: No Active Vehicle";
    }
}

void AscentCC::_stopRecording() {
    if(_vehicle) {
        auto priority_link = _vehicle->priorityLink();
        mavlink_message_t message;
        mavlink_msg_command_long_pack_chan(1, 0, priority_link->mavlinkChannel(), &message, 1, _vehicle->defaultComponentId(), MAV_CMD_ASCENT_PAYLOAD, 0,
                                           2,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN);
        _vehicle->sendMessageOnLink(priority_link, message);
    }
    else{
        qWarning() << "_stopRecord: No Active Vehicle";
    }
}

void AscentCC::_ir(){
    if(_vehicle) {
        auto priority_link = _vehicle->priorityLink();
        mavlink_message_t message;
        mavlink_msg_command_long_pack_chan(1, 0, priority_link->mavlinkChannel(), &message, 1, _vehicle->defaultComponentId(), MAV_CMD_ASCENT_PAYLOAD, 0,
                                           4,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN);
        _vehicle->sendMessageOnLink(priority_link, message);
    }
    else{
        qWarning() << "_ir: No Active Vehicle";
    }
}
//return to x1 digizoom for IR
void AscentCC::_eo() {
    if(_vehicle) {
        auto priority_link = _vehicle->priorityLink();
        mavlink_message_t message;
        mavlink_msg_command_long_pack_chan(1, 0, priority_link->mavlinkChannel(), &message, 1, _vehicle->defaultComponentId(), MAV_CMD_ASCENT_PAYLOAD, 0,
                                           3,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN);
        _vehicle->sendMessageOnLink(priority_link, message);\
    }
    else{
        qWarning() << "_eo: No Active Vehicle";
    }
}

void AscentCC::_eoir() {
    if(_vehicle) {
        auto priority_link = _vehicle->priorityLink();
        mavlink_message_t message;
        mavlink_msg_command_long_pack_chan(1, 0, priority_link->mavlinkChannel(), &message, 1, _vehicle->defaultComponentId(), MAV_CMD_ASCENT_PAYLOAD, 0,
                                           5,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN);
        _vehicle->sendMessageOnLink(priority_link, message);
    }
    else{
        qWarning() << "_eoir: No Active Vehicle";
    }
}

void AscentCC::_ireo() {
    if(_vehicle) {
        auto priority_link = _vehicle->priorityLink();
        mavlink_message_t message;
        mavlink_msg_command_long_pack_chan(1, 0, priority_link->mavlinkChannel(), &message, 1, _vehicle->defaultComponentId(), MAV_CMD_ASCENT_PAYLOAD, 0,
                                           6,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN);
        _vehicle->sendMessageOnLink(priority_link, message);
    }
    else{
        qWarning() << "_ireo: No Active Vehicle";
    }
}

void AscentCC::_nextPalette() {
    if(_vehicle) {
        auto priority_link = _vehicle->priorityLink();
        mavlink_message_t message;
        mavlink_msg_command_long_pack_chan(1, 0, priority_link->mavlinkChannel(), &message, 1, _vehicle->defaultComponentId(), MAV_CMD_ASCENT_PAYLOAD, 0,
                                           7,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN);
        _vehicle->sendMessageOnLink(priority_link, message);
    }
    else{
        qWarning() << "_nextPalette: No Active Vehicle";
    }
}

void AscentCC::_toggleHeat() {
    if(_vehicle) {
        auto priority_link = _vehicle->priorityLink();
        mavlink_message_t message;
        mavlink_msg_command_long_pack_chan(1, 0, priority_link->mavlinkChannel(), &message, 1, _vehicle->defaultComponentId(), MAV_CMD_ASCENT_PAYLOAD, 0,
                                           22,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN);
        _vehicle->sendMessageOnLink(priority_link, message);
    }
    else{
        qWarning() << "_toggleHeat: No Active Vehicle";
    }
}

void AscentCC::_digiZoom1() {
    if(_vehicle) {
        auto priority_link = _vehicle->priorityLink();
        mavlink_message_t message;
        mavlink_msg_command_long_pack_chan(1, 0, priority_link->mavlinkChannel(), &message, 1, _vehicle->defaultComponentId(), MAV_CMD_ASCENT_PAYLOAD, 0,
                                           8,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN);
        _vehicle->sendMessageOnLink(priority_link, message);
    }
    else{
        qWarning() << "_digiZoom1: No Active Vehicle";
    }
}

void AscentCC::_digiZoom2() {
    if(_vehicle) {
        auto priority_link = _vehicle->priorityLink();
        mavlink_message_t message;
        mavlink_msg_command_long_pack_chan(1, 0, priority_link->mavlinkChannel(), &message, 1, _vehicle->defaultComponentId(), MAV_CMD_ASCENT_PAYLOAD, 0,
                                           9,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN);
        _vehicle->sendMessageOnLink(priority_link, message);
    }
    else{
        qWarning() << "_digiZoom2: No Active Vehicle";
    }
}

void AscentCC::_digiZoom4() {
    if(_vehicle) {
        auto priority_link = _vehicle->priorityLink();
        mavlink_message_t message;
        mavlink_msg_command_long_pack_chan(1, 0, priority_link->mavlinkChannel(), &message, 1, _vehicle->defaultComponentId(), MAV_CMD_ASCENT_PAYLOAD, 0,
                                           10,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN);
        _vehicle->sendMessageOnLink(priority_link, message);
    }
    else{
        qWarning() << "_digiZoom4: No Active Vehicle";
    }
}

void AscentCC::_digiZoom8() {
    if(_vehicle) {
        auto priority_link = _vehicle->priorityLink();
        mavlink_message_t message;
        mavlink_msg_command_long_pack_chan(1, 0, priority_link->mavlinkChannel(), &message, 1, _vehicle->defaultComponentId(), MAV_CMD_ASCENT_PAYLOAD, 0,
                                           11,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN);
        _vehicle->sendMessageOnLink(priority_link, message);
    }
    else{
        qWarning() << "_digiZoom8: No Active Vehicle";
    }
}

void AscentCC::_digiZoom16() {
    if(_vehicle) {
        auto priority_link = _vehicle->priorityLink();
        mavlink_message_t message;
        mavlink_msg_command_long_pack_chan(1, 0, priority_link->mavlinkChannel(), &message, 1, _vehicle->defaultComponentId(), MAV_CMD_ASCENT_PAYLOAD, 0,
                                           12,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN);
        _vehicle->sendMessageOnLink(priority_link, message);
    }
    else{
        qWarning() << "_digiZoom16: No Active Vehicle";
    }
}

void AscentCC::_trackPoint(int x, int y) {
    if(_vehicle) {
        auto priority_link = _vehicle->priorityLink();
        mavlink_message_t message;
        mavlink_msg_command_long_pack_chan(1, 0, priority_link->mavlinkChannel(), &message, 1, _vehicle->defaultComponentId(), MAV_CMD_ASCENT_PAYLOAD, 0,
                                           15,
                                           x,
                                           y,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN);
        _vehicle->sendMessageOnLink(priority_link, message);
    }
    else{
        qWarning() << "_trackPoint: No Active Vehicle";
    }
}

void AscentCC::_stopTracking() {
    if(_vehicle) {
        auto priority_link = _vehicle->priorityLink();
        mavlink_message_t message;
        mavlink_msg_command_long_pack_chan(1, 0, priority_link->mavlinkChannel(), &message, 1, _vehicle->defaultComponentId(), MAV_CMD_ASCENT_PAYLOAD, 0,
                                           16,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN);
        _vehicle->sendMessageOnLink(priority_link, message);
    }
    else{
        qWarning() << "_stopTracking: No Active Vehicle";
    }
}

void AscentCC::_lookDown() {
    if(_vehicle) {
        auto priority_link = _vehicle->priorityLink();
        mavlink_message_t message;
        mavlink_msg_command_long_pack_chan(1, 0, priority_link->mavlinkChannel(), &message, 1, _vehicle->defaultComponentId(), MAV_CMD_ASCENT_PAYLOAD, 0,
                                           20,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN);
        _vehicle->sendMessageOnLink(priority_link, message);
    }
    else{
        qWarning() << "_lookDown: No Active Vehicle";
    }
}

void AscentCC::_debug() {
    if(_vehicle) {
        auto priority_link = _vehicle->priorityLink();
        mavlink_message_t message;
        mavlink_msg_command_long_pack_chan(1, 0, priority_link->mavlinkChannel(), &message, 1, _vehicle->defaultComponentId(), MAV_CMD_ASCENT_PAYLOAD, 0,
                                           99,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN);
        // mavlink_msg_command_long_pack_chan(1, 0, priority_link->mavlinkChannel(), &message, 1, _vehicle->defaultComponentId(), MAV_CMD_DO_DIGICAM_CONTROL, 0,
        //                                    NAN,
        //                                    NAN,
        //                                    NAN,
        //                                    NAN,
        //                                    1,
        //                                    NAN,
        //                                    NAN);
        _vehicle->sendMessageOnLink(priority_link, message);
    }
    else{
        qWarning() << "_debug: No Active Vehicle";
    }
}

void AscentCC::_nuc() {
    if(_vehicle) {
        auto priority_link = _vehicle->priorityLink();
        mavlink_message_t message;
        mavlink_msg_command_long_pack_chan(1, 0, priority_link->mavlinkChannel(), &message, 1, _vehicle->defaultComponentId(), MAV_CMD_ASCENT_PAYLOAD, 0,
                                           21,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN,
                                           NAN);
        _vehicle->sendMessageOnLink(priority_link, message);
    }
    else{
        qWarning() << "_NUC: No Active Vehicle";
    }
}

void AscentCC::setIsPiP(bool pip) {
    _isPiP = pip;
    emit isPiPChanged(pip);
}

void AscentCC::setEOIsMain(bool eo) {
    _eoIsMain = eo;
    emit eoIsMainChanged(eo);
}