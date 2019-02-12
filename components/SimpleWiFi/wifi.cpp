//! -------------------------------------------------------------------------------------------- //
//! \file  wifi.cpp
//! \brief This source contains the implementation of the WiFi functions which allow the 
//!        app_main function to connect the esp32 to a designated access point. These functions  
//!        only support station mode.
//!
//!
#include "wifi.h"


//! \brief Globals and constants
//!
static EventGroupHandle_t wifiEventGroup;
const  int                wifiStartBit     = BIT0;
const  int                wifiConnectedBit = BIT1;
const  TickType_t         ticksToWait      = 500 / portTICK_PERIOD_MS;


//! \fn     EventHandler
//! \brief  This static function intercepts system events regarding the wifi 
//!         adapter.
//! \params void* and system_event_t.
//! \return esp_error_t code.
//!
static error EventHandler(void *ctx, system_event_t *event)
{
    switch (event->event_id)
    {
    case SYSTEM_EVENT_STA_START:
        xEventGroupSetBits(wifiEventGroup, wifiStartBit);
        break;
    case SYSTEM_EVENT_STA_CONNECTED:
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifiEventGroup, wifiConnectedBit);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        esp_wifi_connect();
        xEventGroupClearBits(wifiEventGroup, wifiConnectedBit);
        break;
    default:
        break;
    }
    return ESP_OK;
}

//! \fn    WifiInit
//! \brief This function initializes the wifi adapter using mostly 
//!        default values.
//!
void WIFI::WifiInit()
{
    wifi_init_config_t wifiCfg = WIFI_INIT_CONFIG_DEFAULT();
    error              err     = {};

    wifiEventGroup = xEventGroupCreate();
    
    if ((err = nvs_flash_init()) == ESP_ERR_NVS_NO_FREE_PAGES)
    {
        nvs_flash_erase();
        nvs_flash_init();
    }

    tcpip_adapter_init();
    esp_event_loop_init(EventHandler, NULL);
    esp_log_level_set("wifi", ESP_LOG_NONE);
    esp_wifi_init(&wifiCfg);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();

    xEventGroupWaitBits(wifiEventGroup, wifiStartBit, pdFALSE, pdTRUE, portMAX_DELAY);
}

//! \fn    WifiConnect
//! \brief This function connects the esp32 to the configured wifi ssid.
//! \param string ssid, string password.
//!
void WIFI::WifiConnect(string sid, string pwd)
{
    EventBits_t eventBits = {};

    wifi_config_t wifiConfig = {};
    sid.copy((char *)wifiConfig.sta.ssid, sid.length());
    pwd.copy((char *)wifiConfig.sta.password, pwd.length());

    cout << endl << "ESP32 connecting to SSID!" << endl;

    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifiConfig);
    esp_wifi_connect();

    while ((eventBits & wifiConnectedBit) == 0)
    {
        cout << ".";
        cout << std::flush;
        eventBits = xEventGroupWaitBits(wifiEventGroup, wifiConnectedBit, pdFALSE, pdTRUE, ticksToWait);
    }

    cout << "ESP32 connected to SSID!" << endl;
}

//! \fn    WifiDisconnect
//! \brief Disconnects the esp32 from the configured wifi ssid.
//!
void WIFI::WifiDisconnect()
{
    esp_wifi_disconnect();
    xEventGroupClearBits(wifiEventGroup, wifiConnectedBit);
}

//! \fn     WifiGetStatus
//! \brief  Gets the current status of the wifi connection.
//! \return wifiStatus enum value.
//!
wifiStatus WIFI::WifiGetStatus()
{
    if (xEventGroupGetBits(wifiEventGroup) & wifiConnectedBit)
        return WIFI_STATUS_CONNECTED;
    else
        return WIFI_STATUS_DISCONNECTED;
}

//! \fn     WifiIsMeshEnabled
//! \brief  Indicates whether mesh networking is enabled on the calling
//!         node.
//! \return <bool> true of false.
//!
bool WIFI::MESH::WifiIsMeshEnabled()
{
    return false;
}

//! \fn     WiFiMeshIsRoot
//! \brief  Indicates whether the calling node is root node, or not,
//!         when mesh networking is enabled.
//! \return <bool> true or false.
//!
bool WIFI::MESH::WifiIsRootNode()
{
    return true;
}

