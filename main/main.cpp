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

//! \brief Function Prototypes
//!
void PostDataAsyncThread (void *arg);
void ParseRestError      (rerror r);
bool Setup               ();
void CheckBuffer         (int i);

//! \brief Constants
//!
const line BNO_TX = static_cast<line>(21);
const line BNO_RX = static_cast<line>(22);

//! \brief Globals
//!
BnoModule bno;

eventList buffer[2];
mutex     bufferLock[2];

string SSID = {};
string PWD  = {};
string SRV  = {};
string PORT = {};

timerHandle tHandle;
esp_timer_create_args_t timerArgs = {
    &PostDataAsyncThread, 
    0, 
    ESP_TIMER_TASK, 
    "Periodic"
};


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
    /*!< Setup the esp32 and bno055 */
    if (!Setup())
    {
        while (1)
            Pause(10);
    }

    /*!< Success, we've made it to regular output */
    SensorEvent event;

    ESP_ERROR_CHECK(esp_timer_create(&timerArgs, &tHandle));
    ESP_ERROR_CHECK(esp_timer_start_periodic(tHandle, 1000000));
    
    int i = 0;
    while (1)
    {
        int32_t start(0), end(0), elapsed(0), sleep(0);

        start = esp_timer_get_time() / 1000;
        event = bno.GetReading(stringToVector.at(bno.GetTest()));

        if (bufferLock[i].try_lock())
        {
            buffer[i].push_back(event);
            CheckBuffer(i);
            bufferLock[i].unlock();
        }else {
            ++i %= 2;
            bufferLock[i].lock();
            buffer[i].push_back(event);
            CheckBuffer(i);
            bufferLock[i].unlock();
        }

        end     = esp_timer_get_time() / 1000;
        elapsed = end - start;
        sleep   = ((100 - elapsed) > 0 ? (100 - elapsed) : 0);

        Pause(sleep);
    }

    ESP_ERROR_CHECK(esp_timer_stop(tHandle));
    ESP_ERROR_CHECK(esp_timer_delete(tHandle));
}


//! -------------------------------------------------------------------------------------------- //
//! \brief Functions section
//!

//! \fn    PostDataAsyncThread
//! \brief This function runs concurrently with the main thread and posts the readings data
//!        to the server, once every second. It operates using a delta time, to ensure
//!        communication with the server exactly once every second.
//!
void PostDataAsyncThread(void *arg)
{
    static int i = 0;
    rerror     result;

    ESP_ERROR_CHECK(esp_timer_stop(tHandle));

    bufferLock[i].lock();
    if (!buffer[i].empty())
    {
        result = CreateReading(buffer[i]);
        if (result != REST_OK)
            ParseRestError(result);
        buffer[i].clear();
    }
    bufferLock[i].unlock();
    ++i %= 2;

    ESP_ERROR_CHECK(esp_timer_start_periodic(tHandle, 1000000));
}

//! \fn    ParseRestError
//! \brief This function takes the rerror and determines the issue and takes
//!        action.
//! \param <rerror>the rest error.
//!
void ParseRestError(rerror r)
{
    SensorEvent e = {};
    string      json;
    rerror      result = r;

    switch (result)
    {
    case REST_FAIL:
    case REST_OK:
        cout << "REST: success communicating with server." << endl;
        break;
    case REST_REQUEST_ACCEL:
        e    = bno.GetReading(ACCELEROMETER);
        CreateReading(initializer_list<SensorEvent>{e});
        break;
    case REST_REQUEST_MAG:
        e    = bno.GetReading(MAGNETOMETER);
        CreateReading(initializer_list<SensorEvent>{e});
        break;
    case REST_REQUEST_GYRO:
        e    = bno.GetReading(GYROSCOPE);
        CreateReading(initializer_list<SensorEvent>{e});
        break;
    case REST_REQUEST_EULER:
        e    = bno.GetReading(EULER);
        CreateReading(initializer_list<SensorEvent>{e});
        break;
    case REST_REQUEST_LINEARA:
        e    = bno.GetReading(LINEARACCEL);
        CreateReading(initializer_list<SensorEvent>{e});
        break;
    case REST_REQUEST_GRAVITY:
        e    = bno.GetReading(GRAVITY);
        CreateReading(initializer_list<SensorEvent>{e});
        break;
    case REST_CONNECT_FAIL:
        cout << "REST error: couldn't connect to server." << endl;
        break;
    case REST_WRITE_FAIL:
        cout << "REST error: socket error while sending." << endl;
        break;
    case REST_READ_FAIL:
        cout << "REST error: socket error while receiving." << endl;
        break;
    case REST_NO_WIFI:
        cout << "REST error: not connected to WiFi." << endl;
        break;
    case REST_MQTT_ERROR:
        cout << "REST error: an mqtt error has occured." << endl;
        break;
    case REST_SERVER_ERROR:
        cout << "REST error: an error occured with the server." << endl;
    }
}

//! \fn     Setup
//! \brief  This function performs setup for the app_main function, including
//!         initializing UART, creating a BnoModule object, and reading config
//!         options from NVS storage.
//! \return <bool> success or failure.
//!
bool Setup()
{
    /*!< Initialize UART and create BNO object */
    UART::InitUART(UART_NUM_1, BNO_TX, BNO_RX);
    bno = BnoModule(UART_NUM_1, BNO_TX, BNO_RX);

    /*!< BNO setup section, this is critical for normal operation */
    if (!bno.Setup(OPMODE_NDOF))
    {
        cout << "Oops... unable to initialize the BNO055!" << endl;
        return false;
    }else {
        cout << "Success, Found BNO055!" << endl;
    }
    
    /*!< Network setup section, this is necessary for WiFi functionality */
    if (NVS::OpenNVSPartition(NVS_PARTITION_NAME, NVS_NSNAME_NET) == ESP_OK)
    {
        NVS::ReadNetConfig(SSID, PWD, SRV, PORT);
        WIFI::WifiInit();
        WIFI::WifiConnect(SSID, PWD);
    }else {
        cout << "Oops... unable to read net config from NVS!" << endl;
        return false;
    }

    return true;
}

void CheckBuffer(int i)
{
    if (buffer[i].size() > 10)
    {
        auto it = buffer[i].begin();
        buffer[i].erase(it);
    }
}

