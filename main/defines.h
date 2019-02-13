//! -------------------------------------------------------------------------------------------- //
//! \file  defines.h
//! \brief This header contains various data structures and constants used throughout the library.
//!        It contains enumerations, structs, and constants related to the LSM9DS1 inertia module.
//!
//!
#pragma once
#include "includes.h"


//! -------------------------------------------------------------------------------------------- //
//! \brief Typedefs and constants

typedef uint8_t            byte;
typedef uint16_t           word;
typedef int                uerror;
typedef esp_err_t          error;
typedef int8_t             sbyte;
typedef gpio_num_t         line;
typedef uart_port_t        uport;
typedef array<byte, 6>     addr;
typedef vector<string>     strings;
typedef EventGroupHandle_t egHandle;

typedef initializer_list<class SensorEvent*> eventList;

typedef struct testinfo {
    string test;
    word   numTests;
}TESTINFO;

const string POST = "POST /createReading HTTP/1.1\r\n";     /*!< POST field, no further additions needed */
const string HOST = "Host: \r\n";                           /*!< HOST field, insert ip:port at pos 6 */
const string USER = "User-Agent: ESP32\r\n";                /*!< User-Agent, no further additions needed */
const string TYPE = "Content-Type: application/json\r\n";   /*!< Content-Type, nothing further needed */
const string LENG = "Content-Length: \r\n";                 /*!< Content-Length, insert length at pos 16 */
const string CONN = "Connection: Closed\r\n";               /*!< Connection, nothing further needed */
const string NEWL = "\r\n";                                 /*!< newline for end of headers */

const addr rootMacAddr{0x7A, 0x69, 0xDE, 0xAD, 0xBE, 0xEF};
const byte BNO_ADDRESS_A = 0x28;
const byte BNO_ADDRESS_B = 0x29;
const byte BNO_ID        = 0xA0;
const int  LOOPCOUNT     = 16;

const string NVS_PARTITION_NAME = "device_cfg";
const string NVS_NSNAME_CONFIG  = "deviceConfig";
const string NVS_NSNAME_NET     = "netConfig";


//! -------------------------------------------------------------------------------------------- //
//! \brief Enumerations

//! \enum Rest error codes used to accept communication from the server or indicate any
//!       network errors preventing successful communication.
//!
typedef enum {
    REST_FAIL            = -0x01,
    REST_OK              = 0x00,
    REST_CONNECT_FAIL    = 0x01,
    REST_WRITE_FAIL      = 0x02,
    REST_REQUEST_ACCEL   = 0x03,
    REST_REQUEST_MAG     = 0x04,
    REST_REQUEST_GYRO    = 0x05,
    REST_REQUEST_EULER   = 0x06,
    REST_REQUEST_LINEARA = 0x07,
    REST_REQUEST_GRAVITY = 0x08,
    REST_NO_WIFI         = 0x09,
    REST_MQTT_ERROR      = 0x0A
}rerror;

//! \enum Wifi status indicating either connected, or disconnected.
//!
typedef enum {
    WIFI_STATUS_CONNECTED,
    WIFI_STATUS_DISCONNECTED
}wifiStatus;

//! \brief Register addresses of the BNO055
//!
typedef enum
{
    BNO_PAGE_ID_ADDR                 = 0X07,                /*!< Page id register definition */
    BNO_CHIP_ID_ADDR                 = 0x00,                /*!< PAGE0 REGISTER DEFINITION START*/
    BNO_ACCEL_REV_ID_ADDR            = 0x01,
    BNO_MAG_REV_ID_ADDR              = 0x02,
    BNO_GYRO_REV_ID_ADDR             = 0x03,
    BNO_SW_REV_ID_LSB_ADDR           = 0x04,
    BNO_SW_REV_ID_MSB_ADDR           = 0x05,
    BNO_BL_REV_ID_ADDR               = 0X06,
    BNO_ACCEL_DATA_X_LSB_ADDR        = 0X08,                /*!< Accel data register */
    BNO_ACCEL_DATA_X_MSB_ADDR        = 0X09,
    BNO_ACCEL_DATA_Y_LSB_ADDR        = 0X0A,
    BNO_ACCEL_DATA_Y_MSB_ADDR        = 0X0B,
    BNO_ACCEL_DATA_Z_LSB_ADDR        = 0X0C,
    BNO_ACCEL_DATA_Z_MSB_ADDR        = 0X0D,
    BNO_MAG_DATA_X_LSB_ADDR          = 0X0E,                /*!< Mag data register */
    BNO_MAG_DATA_X_MSB_ADDR          = 0X0F,
    BNO_MAG_DATA_Y_LSB_ADDR          = 0X10,
    BNO_MAG_DATA_Y_MSB_ADDR          = 0X11,
    BNO_MAG_DATA_Z_LSB_ADDR          = 0X12,
    BNO_MAG_DATA_Z_MSB_ADDR          = 0X13,
    BNO_GYRO_DATA_X_LSB_ADDR         = 0X14,                /*!< Gyro data registers */
    BNO_GYRO_DATA_X_MSB_ADDR         = 0X15,
    BNO_GYRO_DATA_Y_LSB_ADDR         = 0X16,
    BNO_GYRO_DATA_Y_MSB_ADDR         = 0X17,
    BNO_GYRO_DATA_Z_LSB_ADDR         = 0X18,
    BNO_GYRO_DATA_Z_MSB_ADDR         = 0X19,
    BNO_EULER_H_LSB_ADDR             = 0X1A,                /*!< Euler data registers */
    BNO_EULER_H_MSB_ADDR             = 0X1B,
    BNO_EULER_R_LSB_ADDR             = 0X1C,
    BNO_EULER_R_MSB_ADDR             = 0X1D,
    BNO_EULER_P_LSB_ADDR             = 0X1E,
    BNO_EULER_P_MSB_ADDR             = 0X1F,
    BNO_QUATERNION_DATA_W_LSB_ADDR   = 0X20,                /*!< Quaternion data registers */
    BNO_QUATERNION_DATA_W_MSB_ADDR   = 0X21,
    BNO_QUATERNION_DATA_X_LSB_ADDR   = 0X22,
    BNO_QUATERNION_DATA_X_MSB_ADDR   = 0X23,
    BNO_QUATERNION_DATA_Y_LSB_ADDR   = 0X24,
    BNO_QUATERNION_DATA_Y_MSB_ADDR   = 0X25,
    BNO_QUATERNION_DATA_Z_LSB_ADDR   = 0X26,
    BNO_QUATERNION_DATA_Z_MSB_ADDR   = 0X27,
    BNO_LINEAR_ACCEL_DATA_X_LSB_ADDR = 0X28,                /*!< Linear acceleration data registers */
    BNO_LINEAR_ACCEL_DATA_X_MSB_ADDR = 0X29,
    BNO_LINEAR_ACCEL_DATA_Y_LSB_ADDR = 0X2A,
    BNO_LINEAR_ACCEL_DATA_Y_MSB_ADDR = 0X2B,
    BNO_LINEAR_ACCEL_DATA_Z_LSB_ADDR = 0X2C,
    BNO_LINEAR_ACCEL_DATA_Z_MSB_ADDR = 0X2D,
    BNO_GRAVITY_DATA_X_LSB_ADDR      = 0X2E,                /*!< Gravity data registers */
    BNO_GRAVITY_DATA_X_MSB_ADDR      = 0X2F,
    BNO_GRAVITY_DATA_Y_LSB_ADDR      = 0X30,
    BNO_GRAVITY_DATA_Y_MSB_ADDR      = 0X31,
    BNO_GRAVITY_DATA_Z_LSB_ADDR      = 0X32,
    BNO_GRAVITY_DATA_Z_MSB_ADDR      = 0X33,
    BNO_TEMP_ADDR                    = 0X34,                /*!< Temperature data register */
    BNO_CALIB_STAT_ADDR              = 0X35,                /*!< Status registers */
    BNO_SELFTEST_RESULT_ADDR         = 0X36,
    BNO_INTR_STAT_ADDR               = 0X37,
    BNO_SYS_CLK_STAT_ADDR            = 0X38,
    BNO_SYS_STAT_ADDR                = 0X39,
    BNO_SYS_ERR_ADDR                 = 0X3A,
    BNO_UNIT_SEL_ADDR                = 0X3B,                /*!< Unit selection register */
    BNO_DATA_SELECT_ADDR             = 0X3C,
    BNO_OPR_MODE_ADDR                = 0X3D,                /*!< Mode registers */
    BNO_PWR_MODE_ADDR                = 0X3E,
    BNO_SYS_TRIGGER_ADDR             = 0X3F,
    BNO_TEMP_SOURCE_ADDR             = 0X40,
    BNO_AXIS_MAP_CONFIG_ADDR         = 0X41,                /*!< Axis remap registers */
    BNO_AXIS_MAP_SIGN_ADDR           = 0X42,
    ACCEL_OFFSET_X_LSB_ADDR          = 0X55,                /*!< Accelerometer Offset registers */
    ACCEL_OFFSET_X_MSB_ADDR          = 0X56,
    ACCEL_OFFSET_Y_LSB_ADDR          = 0X57,
    ACCEL_OFFSET_Y_MSB_ADDR          = 0X58,
    ACCEL_OFFSET_Z_LSB_ADDR          = 0X59,
    ACCEL_OFFSET_Z_MSB_ADDR          = 0X5A,
    MAG_OFFSET_X_LSB_ADDR            = 0X5B,                /*!< Magnetometer Offset registers */
    MAG_OFFSET_X_MSB_ADDR            = 0X5C,
    MAG_OFFSET_Y_LSB_ADDR            = 0X5D,
    MAG_OFFSET_Y_MSB_ADDR            = 0X5E,
    MAG_OFFSET_Z_LSB_ADDR            = 0X5F,
    MAG_OFFSET_Z_MSB_ADDR            = 0X60,
    GYRO_OFFSET_X_LSB_ADDR           = 0X61,                /*!< Gyroscope Offset register s*/
    GYRO_OFFSET_X_MSB_ADDR           = 0X62,
    GYRO_OFFSET_Y_LSB_ADDR           = 0X63,
    GYRO_OFFSET_Y_MSB_ADDR           = 0X64,
    GYRO_OFFSET_Z_LSB_ADDR           = 0X65,
    GYRO_OFFSET_Z_MSB_ADDR           = 0X66,
    ACCEL_RADIUS_LSB_ADDR            = 0X67,                /*!< Radius registers */
    ACCEL_RADIUS_MSB_ADDR            = 0X68,
    MAG_RADIUS_LSB_ADDR              = 0X69,
    MAG_RADIUS_MSB_ADDR              = 0X6A
}bnoRegister;

typedef enum
{
    POWER_MODE_NORMAL   = 0X00,
    POWER_MODE_LOWPOWER = 0X01,
    POWER_MODE_SUSPEND  = 0X02
}bnoPowermode;

typedef enum
{
    OPMODE_CONFIG       = 0X00,
    OPMODE_ACCONLY      = 0X01,
    OPMODE_MAGONLY      = 0X02,
    OPMODE_GYRONLY      = 0X03,
    OPMODE_ACCMAG       = 0X04,
    OPMODE_ACCGYRO      = 0X05,
    OPMODE_MAGGYRO      = 0X06,
    OPMODE_AMG          = 0X07,
    OPMODE_IMUPLUS      = 0X08,
    OPMODE_COMPASS      = 0X09,
    OPMODE_M4G          = 0X0A,
    OPMODE_NDOF_FMC_OFF = 0X0B,
    OPMODE_NDOF         = 0X0C
}bnoOpmode;

//! \brief bnoAxis allows the SetAxisSign function to specify which axis'
//!        sign is being set, either positive or negative.
//!
typedef enum
{
    Z = 0x00,
    Y = 0x01,
    X = 0x02
}bnoAxis;

//! \brief bnoAxisRemapConfig allows the SetAxisRemap function to specify
//!        which configuration to apply to all three axes. Either linear shift
//!        left or right (e.g. x=z, y=x, z=y) or swapping two individual axes.
//!
typedef enum
{
    REMAP_SHIFT_LEFT    = 0X12,
    REMAP_SHIFT_RIGHT   = 0X09,
    REMAP_SWITCH_XY     = 0X21,
    REMAP_SWITCH_YZ     = 0X18,
    REMAP_SWITCH_ZX     = 0X06
}bnoAxisRemapConfig;

//! \brief bnoAxisRemapSign enumerates the two possible signs for an axis to be
//!        set to, positive or negative.
//!
typedef enum
{
    REMAP_AXIS_POSITIVE = 0x00,
    REMAP_AXIS_NEGATIVE = 0x01
}bnoAxisRemapSign;

//! \brief Location refers to the position on the body where the sensor is
//!        placed. 0x00 is the chest and acts as superpeer to the remaining
//!        sensors. All sensors on the right side of the body are odd numbers
//!        while the left side sensors are even.
//!
typedef enum: byte
{
    locChest            = 0x00,
    locRightArmUpper    = 0x01,
    locLeftArmUpper     = 0x02,
    locRightArmLower    = 0x03,
    locLeftArmLower     = 0x04,
    locRightThigh       = 0x05,
    locLeftThigh        = 0x06,
    locRightShin        = 0x07,
    locLeftShin         = 0x08
}devLocation;

typedef enum
{
    ACCELEROMETER       = BNO_ACCEL_DATA_X_LSB_ADDR,
    MAGNETOMETER        = BNO_MAG_DATA_X_LSB_ADDR,
    GYROSCOPE           = BNO_GYRO_DATA_X_LSB_ADDR,
    EULER               = BNO_EULER_H_LSB_ADDR,
    QUATERNION          = BNO_QUATERNION_DATA_W_LSB_ADDR,
    LINEARACCEL         = BNO_LINEAR_ACCEL_DATA_X_LSB_ADDR,
    GRAVITY             = BNO_GRAVITY_DATA_X_LSB_ADDR
}bnoVectorType;


//! -------------------------------------------------------------------------------------------- //
//! \brief Structs and Classes

//! \brief Const map of vector types to strings to have a 
//!        human readable version of each vector.
//!
const map<bnoVectorType, string> vectorToString = {
    {ACCELEROMETER, "Accel"},
    {MAGNETOMETER,  "Mag"},
    {GYROSCOPE,     "Gyro"},
    {EULER,         "Euler"},
    {LINEARACCEL,   "LinearAccel"},
    {GRAVITY,       "Gravity"}
};

const map<string, bnoVectorType> stringToVector = {
    {"Accel",       ACCELEROMETER},
    {"Mag",         MAGNETOMETER},
    {"Gyro",        GYROSCOPE},
    {"Euler",       EULER},
    {"Quaternion",  QUATERNION},
    {"LinearAccel", LINEARACCEL},
    {"Gravity",     GRAVITY}
};

const map<devLocation, string> locationToString = {
    {locChest,         "Chest"},
    {locRightArmUpper, "RightArmUpper"},
    {locLeftArmUpper,  "LeftArmUpper"},
    {locRightArmLower, "RightArmLower"},
    {locLeftArmLower,  "LeftArmLower"},
    {locRightThigh,    "RightThigh"},
    {locLeftThigh,     "LeftThigh"},
    {locRightShin,     "RightShin"},
    {locLeftShin,      "LeftShin"}
};

typedef struct
{
    uint8_t  accelRev;
    uint8_t  magRev;
    uint8_t  gyroRev;
    uint16_t swRev;
    uint8_t  blRev;
}bnoRevInfo;


//! \class Quaternion
//! \brief A class representing a single quaternion as received from 
//!        the bno055 imu. Values included are scaler w, and the vector 
//!        x, y, and z.
//!
class Quaternion
{
public:
    Quaternion(){}
    Quaternion(double _x, double _y, double _z) { 
        x = _x; y = _y; z = _z; isQuaternion = false;
    }
    Quaternion(double _w, double _x, double _y, double _z) { 
        w = _w; x = _x; y = _y; z = _z; isQuaternion = true;
    }
    
    bool   IsQuaternion() { return isQuaternion; }
    double GetEventW()    { return w; }
    double GetEventX()    { return x; }
    double GetEventY()    { return y; }
    double GetEventZ()    { return z; }
    
private:
    double w, x, y, z;
    bool   isQuaternion;
};

