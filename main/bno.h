//! -------------------------------------------------------------------------------------------- //
//! \file  bno.h
//! \brief This header contains the definition of the BnoModule class which is the main object 
//!        representing the inertia module. It allows a user to create a module object, poll the 
//!        sensors by calling "GetEvent" and the ability to set individual sensor 
//!        characteristics.
//!
//!
#pragma once
#include "event.h"
#include "sparkfun.h"


//! \class BnoModule bno.h
//! \brief The BnoModule class encapsulates the BNO055 inertia module, which contains 
//!        three sensors: the accelerometer, gyroscope, and magnetometer. It can also 
//!        take temperature readings. The class exposes several setup functions to the 
//!        user, and a GetReading function, which returns a reading.
class BnoModule
{
public:
    BnoModule(){}
    BnoModule(uport p, line tx, line rx);
    
    bool         Setup     (bnoOpmode mode);
    SensorEvent* GetReading(bnoVectorType typeOfData = QUATERNION);

    /*!< inline public methods */
    bool         IsRoot    () { return deviceId == 1000; }
    bool         IsTest    () { return !test.test.empty(); }
    TESTINFO     GetTest   () { return test; }
    /*!< inline public methods */

private:
    Quaternion*  ReadQuat  ();
    Quaternion*  ReadVector(bnoVectorType whichSensor);

    uerror SetAxisRemap(bnoAxisRemapConfig config);
    uerror SetAxisSign(bnoAxisRemapSign sign, bnoAxis axis);
    uerror SetPwrMode(bnoPowermode mode);
    uerror SetOprMode(bnoOpmode mode);
    uerror DigitalRead (bnoRegister reg, byte *buff, byte len);
    uerror DigitalWrite(bnoRegister reg, byte value, byte len);

    /*<! Private Data Section */
    byte        location;
    word        deviceId;
    line        txPin, rxPin;
    uport       uaPort;
    SensorEvent *event;

    TESTINFO test;
};

