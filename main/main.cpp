//! -------------------------------------------------------------------------------------------- //
//! \file  main.cpp
//! \brief This source contains the main function and entry point to the library.
//!
//!
#include "bno.h"
#include "rest.h"


//! -------------------------------------------------------------------------------------------- //
//! \brief Globals, constants, and function declarations section.
//!
BnoModule bno;

const line BNO_TX = static_cast<line>(21);
const line BNO_RX = static_cast<line>(22);

string SSID = {};
string PWD  = {};
string SRV  = {};
string PORT = {};

void ParseRestError(rerror r);
void RunTest(TESTINFO ti);


//! -------------------------------------------------------------------------------------------- //
//! \brief Main section
//!

//! \fn    app_main
//! \brief This is the main entry point of the freeRtos app. First the wifi driver 
//!        is initialized, and the access point connect to, then the LsmModule object 
//!        is created to be able to take readings and post to the server.
//!
extern "C" void app_main()
{
    /*!< Network setup section, this is necessary for WiFi functionality */
    if (NVS::OpenNVSPartition(NVS_PARTITION_NAME, NVS_NSNAME_NET) == ESP_OK)
    {
        NVS::ReadNetConfig(SSID, PWD, SRV, PORT);
        WIFI::WifiInit();
        WIFI::WifiConnect(SSID, PWD);
    }else {
        cout << "Oops... unable to read net config from NVS!" << endl;
        while (1)
            Pause(10);
    }
    
    /*!< Initialize UART and create BNO object */
    UART::InitUART(UART_NUM_1, BNO_TX, BNO_RX);
    bno = BnoModule(UART_NUM_1, BNO_TX, BNO_RX);

    /*!< BNO setup section, this is critical for normal operation */
    if (!bno.Setup(OPMODE_NDOF))
    {
        cout << "Oops... unable to initialize the BNO055!" << endl;
        while (1)
            Pause(10);
    }else {
        cout << "Success, Found BNO055!" << endl;
    }

    /*!< Testing section, execution will not proceed past here */
    if (bno.IsTest())
    {
        RunTest(bno.GetTest());
        while (1)
            Pause(10);
    }

    /*!< Success, we've made it to regular output */
    SensorEvent *quatern;
    string      jsonData;
    rerror      result;
    
    while (1)
    {
        quatern = bno.GetReading();

        result  = CreateReading(initializer_list<SensorEvent*>{quatern});
        if (result != REST_OK)
            ParseRestError(result);
        
        Pause(200);
    }
}


//! -------------------------------------------------------------------------------------------- //
//! \brief Functions section
//!

//! \fn    ParseRestError
//! \brief This function takes the rerror and determines the issue and takes
//!        action.
//! \param <rerror>the rest error.
//!
void ParseRestError(rerror r)
{
    SensorEvent *e;
    string      json;
    rerror      result = r;

    switch (result)
    {
    case REST_FAIL:
    case REST_OK:
        break;
    case REST_CONNECT_FAIL:
        cout << "REST error: couldn't connect to server." << endl;
        break;
    case REST_WRITE_FAIL:
        cout << "REST error: socket error while sending." << endl;
        break;
    case REST_REQUEST_ACCEL:
        e    = bno.GetReading(ACCELEROMETER);
        CreateReading(initializer_list<SensorEvent*>{e});
        break;
    case REST_REQUEST_MAG:
        e    = bno.GetReading(MAGNETOMETER);
        CreateReading(initializer_list<SensorEvent*>{e});
        break;
    case REST_REQUEST_GYRO:
        e    = bno.GetReading(GYROSCOPE);
        CreateReading(initializer_list<SensorEvent*>{e});
        break;
    case REST_REQUEST_EULER:
        e    = bno.GetReading(EULER);
        CreateReading(initializer_list<SensorEvent*>{e});
        break;
    case REST_REQUEST_LINEARA:
        e    = bno.GetReading(LINEARACCEL);
        CreateReading(initializer_list<SensorEvent*>{e});
        break;
    case REST_REQUEST_GRAVITY:
        e    = bno.GetReading(GRAVITY);
        CreateReading(initializer_list<SensorEvent*>{e});
        break;
    case REST_NO_WIFI:
        cout << "REST error: not connected to WiFi." << endl;
        break;
    case REST_MQTT_ERROR:
        cout << "REST error: an mqtt error has occured." << endl;
        break;
    }
}

//! \fn     RunTest
//! \brief  This function accepts test parameters as input and performs the specified
//!         test. Test parameters dictate the requested reading, e.g. accelerometer or
//!         gyroscope, and how many readings.
//! \params <TESTINFO> struct containing test and numTests
//!
void RunTest(TESTINFO ti)
{
    SensorEvent *event;
    string      jsonData;
    rerror      result;

    for (int i = 0; i < ti.numTests; i++)
    {
        event    = bno.GetReading(stringToVector.at(ti.test));
        result   = CreateReading(initializer_list<SensorEvent*>{event});
        if (result != REST_OK)
            ParseRestError(result);
        Pause(50);
    }
}

