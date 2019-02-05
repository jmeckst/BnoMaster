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
static const uint8_t      MESH_ID[6]       = { 0x77, 0x77, 0x77, 0x77, 0x77, 0x77 };
static EventGroupHandle_t meshEventGroup;
const  int                meshStartBit     = BIT0;
const  int                meshConnectedBit = BIT1;

extern BnoModule bno;


//! \fn     EventHandler
//! \brief  This static function intercepts system events regarding the wifi 
//!         adapter.
//! \params void* and system_event_t.
//! \return esp_error_t code.
//!
static error MeshEventHandler(mesh_event_t event)
{
    
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
    tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP);
    tcpip_adapter_dhcpc_stop(TCPIP_ADAPTER_IF_STA);
    esp_event_loop_init(NULL, NULL);

    esp_wifi_init(&wifiCfg);
    esp_wifi_set_storage(WIFI_STORAGE_FLASH);
    esp_wifi_start();

    esp_mesh_init();
    esp_mesh_set_max_layer(1);
    esp_mesh_fix_root(1);
    if (bno.IsRoot())
        esp_mesh_set_type(MESH_ROOT);
}

//! \fn    WifiConnect
//! \brief This function connects the esp32 to the configured wifi ssid.
//! \param string ssid, string password.
//!
void WIFI::WifiConnect(string sid, string pwd)
{
    mesh_cfg_t meshCfg = MESH_INIT_CONFIG_DEFAULT();
    string     meshPwd(CONFIG_MESH_AP_PASSWD);

    meshCfg.event_cb = &MeshEventHandler;
    meshCfg.channel  = CONFIG_MESH_CHANNEL;
    meshCfg.router.ssid_len = sid.length();
    meshCfg.mesh_ap.max_connection = CONFIG_MESH_AP_CONNECTIONS;

    CopyMemory(&meshCfg.mesh_id, MESH_ID, 6);
    CopyMemory(&meshCfg.router.ssid, sid.c_str(), sid.length());
    CopyMemory(&meshCfg.router.password, pwd.c_str(), pwd.length());
    CopyMemory(&meshCfg.mesh_ap.password, meshPwd.c_str(), meshPwd.length());
    
    esp_mesh_set_ap_authmode(CONFIG_MESH_AP_AUTHMODE);
    esp_mesh_set_config(&cfg);
    esp_mesh_start();
}

//! \fn    WifiDisconnect
//! \brief Disconnects the esp32 from the configured wifi ssid.
//!
void WIFI::WifiDisconnect()
{
    esp_mesh_stop();
}

//! \fn     WifiGetStatus
//! \brief  Gets the current status of the wifi connection.
//! \return wifiStatus enum value.
//!
wifiStatus WIFI::WifiGetStatus()
{
    if (xEventGroupGetBits(meshEventGroup) & meshConnectedBit)
        return WIFI_STATUS_CONNECTED;
    else
        return WIFI_STATUS_DISCONNECTED;
}

