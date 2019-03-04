//! -------------------------------------------------------------------------------------------- //
//! \file  bno.cpp
//! \brief This source file contains the implementation of the BnoModule class which is the main  
//!        object representing the inertia module. It allows a user to create a module object,  
//!        poll the sensors by calling "GetEvent," and the ability to set individual sensor 
//!        characteristics or just use fusion mode to get quaternions.
//!
//!
#include "bno.h"


//! \fn       Constructor
//! \memberof BnoModule
//! \brief    This is the constructor for the BnoModule class. It accepts three 
//!           input parameters: the clock line, data line, and slave address.
//! \param    <line> clock and data lines, <byte> slave address.
//!
BnoModule::BnoModule(uport p, line tx, line rx)
{
    uaPort = p;
    txPin  = tx;
    rxPin  = rx;
}

//! \fn       Setup
//! \memberof BnoModule
//! \brief    The setup function writes values to the necessary bno registers to 
//!           initialize the imu.
//! \param    <bnoOpmode> the requested mode of operation.
//! \return   <bool> for success or failure.
//!
bool BnoModule::Setup(bnoOpmode mode)
{
    byte id = {};

    /*!< First read the device config partition */
    if (NVS::OpenNVSPartition(NVS_PARTITION_NAME, NVS_NSNAME_CONFIG) == ESP_OK)
    {
        NVS::ReadDeviceConfig(location, deviceId, test);
    }else {
        cout << "Oops... unable to read device config from NVS!" << endl;
        return false;
    }

    /*!< Check for the correct chip id of the BNO055 */
    DigitalWrite(BNO_PAGE_ID_ADDR, 0, 1);
    DigitalRead(BNO_CHIP_ID_ADDR, &id, 1);
    if (id != BNO_ID)
        return false;

    /*!< Begin setup of power mode and other configurations */
    SetOprMode(OPMODE_CONFIG);

    DigitalWrite(BNO_SYS_TRIGGER_ADDR, 0x20, 1);
    Pause(650);

    SetPwrMode(POWER_MODE_NORMAL);
    Pause(10);

    DigitalWrite(BNO_PAGE_ID_ADDR, 0, 1);

    /*!< Configure axis mapping here */
    if (location == locChest)
    {
        SetAxisRemap(REMAP_SWITCH_YZ);
        SetAxisSign(REMAP_AXIS_NEGATIVE, bnoAxis::X);
    }else if (location % 2 == 1) {                      /*!< Right side */
        SetAxisRemap(REMAP_SHIFT_LEFT);
    }else if (location % 2 == 0) {                      /*!< Left side */
        SetAxisRemap(REMAP_SHIFT_LEFT);
        SetAxisSign(REMAP_AXIS_NEGATIVE, bnoAxis::X);
        SetAxisSign(REMAP_AXIS_NEGATIVE, bnoAxis::Y);
    }

    DigitalWrite(BNO_SYS_TRIGGER_ADDR, 0x0, 1);
    Pause(10);

    /*!< Set to requested mode of operation */
    SetOprMode(mode);
    Pause(150);

    return true;
}

//! \fn       GetReading
//! \memberof BnoModule
//! \brief    GetReading initiates retrieving an event that occurs on the IMU, 
//!           which is a reading of either an individual sensor, or if fusion 
//!           mode is turned on, a vector or quaternion.
//! \param    <SensorEvent> a pointer, <bnoVectorType> the sensor to read.
//! \return   <bool> success or failure.
//!
SensorEvent* BnoModule::GetReading(bnoVectorType typeOfData)
{
    SensorEvent *e;
    Quaternion  *obj;

    e = new SensorEvent();
    e->SetLocation(location);

    switch (typeOfData)
    {
    case ACCELEROMETER:
    case MAGNETOMETER:
    case GYROSCOPE:
        SetOprMode(OPMODE_AMG);
        obj = ReadVector(typeOfData);
        e->SetObj(obj);
        e->SetName(vectorToString.at(typeOfData));
        break;
    case QUATERNION:
        SetOprMode(OPMODE_NDOF);
        obj = ReadQuat();
        e->SetObj(obj);
        e->SetName("Quaternion");
        break;
    case EULER:
    case LINEARACCEL:
    case GRAVITY:
        SetOprMode(OPMODE_NDOF);
        obj = ReadVector(typeOfData);
        e->SetObj(obj);
        e->SetName(vectorToString.at(typeOfData));
        break;
    default:
        break;
    }

    return e;
}

//! \fn       ReadQuat
//! \memberof BnoModule
//! \brief    ReadQuat performs a read from the Bno055 IMU using the fusion mode
//!           quaternion memory address and reads 8 bytes into a buffer. A quaternion
//!           object is created and returned to the user.
//! \return   <Quaternion*> an object pointer.
//!
Quaternion* BnoModule::ReadQuat()
{
    byte buffer[8];
    ZeroMemory(buffer, 8);

    int16_t w(0), x(0), y(0), z(0);

    DigitalRead(BNO_QUATERNION_DATA_W_LSB_ADDR, buffer, 8);

    w = (((uint16_t)buffer[1]) << 8) | ((uint16_t)buffer[0]);
    x = (((uint16_t)buffer[3]) << 8) | ((uint16_t)buffer[2]);
    y = (((uint16_t)buffer[5]) << 8) | ((uint16_t)buffer[4]);
    z = (((uint16_t)buffer[7]) << 8) | ((uint16_t)buffer[6]);

    const double scale = (1.0 / (1<<14));

    return new Quaternion(scale * w, scale * x, scale * y, scale * z);
}

//! \fn       ReadVector
//! \memberof BnoModule
//! \brief    ReadVector performs a read from the Bno055 IMU using the non-fusion mode
//!           vector memory address specified by 'whichSensor' and reads 6 bytes into a 
//!           buffer. A quaternion object, minus the scalar portion, is created and 
//!           returned to the user.
//! \param    <bnoVectorType> the sensor memory address.
//! \return   <Quaternion*> an object pointer.
//!
Quaternion* BnoModule::ReadVector(bnoVectorType whichSensor)
{
    byte buffer[6];
    ZeroMemory(buffer, 6);

    int16_t x(0), y(0), z(0);

    DigitalRead(static_cast<bnoRegister>(whichSensor), buffer, 6);

    x = (((int16_t)buffer[1]) << 8) | ((int16_t)buffer[0]);
    y = (((int16_t)buffer[3]) << 8) | ((int16_t)buffer[2]);
    z = (((int16_t)buffer[5]) << 8) | ((int16_t)buffer[4]);

    switch(whichSensor)
    {
    case MAGNETOMETER:  /* 1uT = 16 LSB */
    case GYROSCOPE:     /* 1dps = 16 LSB */
    case EULER:         /* 1 degree = 16 LSB */
        x = ((double)x)/16.0;
        y = ((double)y)/16.0;
        z = ((double)z)/16.0;
        break;
    case ACCELEROMETER:
    case LINEARACCEL:
    case GRAVITY:       /* 1m/s^2 = 100 LSB */
        x = ((double)x)/100.0;
        y = ((double)y)/100.0;
        z = ((double)z)/100.0;
        break;
    case QUATERNION:
        break;
    }

    return new Quaternion(x, y, z);
}

//! \fn       SetAxisRemap
//! \memberof BnoModule
//! \brief    This function changes the axis mapping of the IMU, which could
//!           be either the x, y, or z axis. Available configs are shifting 
//!           each axis up or down one, or swapping one with another.
//!           e.g. X=Y, Y=Z, Z=X; or X=Y, Y=X, Z=Z
//! \param    <bnoAxisRemapConfig> the preset config
//! \return   <uerror> UART error code
//!
uerror BnoModule::SetAxisRemap(bnoAxisRemapConfig config)
{
    uerror result = {};
    result = DigitalWrite(BNO_AXIS_MAP_CONFIG_ADDR, static_cast<byte>(config), 1);
    Pause(10);

    return result;
}

//! \fn       SetAxisSign
//! \memberof BnoModule
//! \brief    This function changes the axis sign of the IMU, which could
//!           be either the x, y, or z sign. The axis parameter indicates
//!           which position in the register the value will be written. The
//!           sign parameter is then shifted that many places to the left.
//!           e.g. register mapping is bit2=X, bit1=Y, bit0=Z. To set X to
//!           negative sign shift (1 << 2).
//! \param    <bnoAxisRemapSign> the preset config
//! \return   <uerror> UART error code
//!
uerror BnoModule::SetAxisSign(bnoAxisRemapSign sign, bnoAxis axis)
{
    uerror result = {};
    byte   shift  = (sign << axis);
    result = DigitalWrite(BNO_AXIS_MAP_SIGN_ADDR, shift, 1);
    Pause(10);

    return result;
}

//! \fn       SetPwrMode
//! \memberof BnoModule
//! \brief    This function sets the power mode of the BNO055 IMU. The options are 
//!           normal, low power, and suspend modes.
//! \param    <bnoPowerMode> mode to set.
//!
uerror BnoModule::SetPwrMode(bnoPowermode mode)
{
    uerror result = {};
    result = DigitalWrite(BNO_PWR_MODE_ADDR, mode, 1);
    Pause(30);

    return result;
}

//! \fn       SetOprMode
//! \memberof BnoModule
//! \brief    This function sets the operating mode of the BNO055 IMU. The options are
//!           config, accel only, mag only, gyro only, combined sensors, or fusion. See
//!           defines.h for full list.
//! \param    <bnoOpmode> mode to set.
//!
uerror BnoModule::SetOprMode(bnoOpmode mode)
{
    uerror result = {};
    result = DigitalWrite(BNO_OPR_MODE_ADDR, mode, 1);
    Pause(30);

    return result;
}

//! \fn       DigitalRead
//! \memberof BnoModule
//! \brief    This function takes the bno register address to be read from, as well as
//!           a buffer to read data into, and sends the command to the IMU. If 'len' is 
//!           only 1 a one-byte read will be performed, otherwise multi-byte.
//! \param    <bnoRegister> register, <byte> buffer and length.
//! \return   <error> error code.
//!
uerror BnoModule::DigitalRead(bnoRegister reg, byte *buff, byte len)
{
    uerror result = {};
    byte   cmd[4] = {};
    byte   *data  = new byte[len + 2];

    cmd[0] = 0xAA;
    cmd[1] = 0x01;
    cmd[2] = static_cast<byte>(reg);
    cmd[3] = len;

    for (int i = 0; i < UARTLOOPCOUNT; i++)
    {
        uart_flush(uaPort);
        uart_write_bytes(uaPort, (const char *)cmd, 4);

        if (uart_read_bytes(uaPort, data, len + 2, 20 / portTICK_PERIOD_MS) > 0)
        {
            switch (data[0])
            {
            case 0xBB:
                CopyMemory(buff, data + 2, len);
                result = data[0];
                break;
            case 0xEE:
                result = data[1];
                break;
            }

            if (CompareTo(result, {0xBB}))                        /*!< 0xBB means success */
                break;
            else if (CompareTo(result, {0x02, 0x06, 0x07, 0x0A})) /*!< Here there is maybe a serial issue */
                continue;
            else                                                  /*!< Everything else, an issue with the parameters */
                break;
        }
    }
    delete data;

    return result;
}

//! \fn       DigitalWrite
//! \memberof BnoModule
//! \brief    This function takes the bno register address to be written to, as well as
//!           the value to write, and sends the command to the IMU.
//! \param    <bnoRegister> register, <byte> value and length.
//! \return   <error> error code.
//!
uerror BnoModule::DigitalWrite(bnoRegister reg, byte value, byte len)
{
    uerror result  = {};
    byte   *cmd    = new byte[len + 4];
    byte   resp[2] = {};

    cmd[0] = 0xAA;
    cmd[1] = 0x00;
    cmd[2] = static_cast<byte>(reg);
    cmd[3] = len;
    CopyMemory(cmd + 4, &value, len);

    for (int i = 0; i < UARTLOOPCOUNT; i++)
    {
        uart_flush(uaPort);
        uart_write_bytes(uaPort, (const char *)cmd, len + 4);

        if (uart_read_bytes(uaPort, resp, 2, 20 / portTICK_PERIOD_MS) > 0)
        {
            result = resp[1];

            if (CompareTo(result, {0x01}))                          /*!< 0x01 means success */
                break;
            else if (CompareTo(result, {0x03, 0x06, 0x07, 0x0A}))   /*!< Here there is maybe a serial issue */
                continue;
            else                                                    /*!< Everything else, an issue with the parameters */
                break;
        }
    }
    delete cmd;

    return result;
}

