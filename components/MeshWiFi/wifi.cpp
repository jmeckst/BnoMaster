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
egHandle         meshEventGroup   = {};
const int        meshStartBit     = BIT0;
const int        meshConnectedBit = BIT1;
const TickType_t ticksToWait      = 500 / portTICK_PERIOD_MS;

string routerSSID;
string routerPSWD;
addr   macAddr;

extern BnoModule bno;


void ScanHandler(int num)
{
    int  ieLen       = 0;
    bool parentFound = false;
    mesh_type_t        myType       = MESH_IDLE;
    mesh_assoc_t       assoc        = {};
    mesh_assoc_t       parentAssoc  = {};
    wifi_ap_record_t   record       = {};
    wifi_ap_record_t   parentRecord = {};
    wifi_config_t      parent       = {};
    wifi_scan_config_t scanConfig   = {};

    for (int i = 0; i < num; i++) {
        esp_mesh_scan_get_ap_ie_len(&ieLen);
        esp_mesh_scan_get_ap_record(&record, &assoc);
        if (ieLen == sizeof(assoc) && !esp_mesh_is_root())
        {
            array<byte, 6> tempAddr;
            copy(begin(assoc.mesh_id), end(assoc.mesh_id), begin(tempAddr));
            if (tempAddr == rootMacAddr)
            {
                parentFound = true;
                CopyMemory(&parentRecord, &record, sizeof(record));
                CopyMemory(&parentAssoc, &assoc, sizeof(assoc));
                myType = MESH_LEAF;
                break;
            }
        }else if (ieLen != sizeof(assoc) && esp_mesh_is_root()) {
            string sid((char *)record.ssid);
            if (sid.compare(routerSSID) == 0)
            {
                parentFound = true;
                myType      = MESH_ROOT;
                CopyMemory(&parentRecord, &record, sizeof(record));
                break;
            }
        }
    }
    esp_mesh_flush_scan_result();

    if (parentFound)
    {
        parent.sta.channel = parentRecord.primary;
        CopyMemory(&parent.sta.ssid, &parentRecord.ssid, sizeof(parentRecord.ssid));
        parent.sta.bssid_set = 1;
        CopyMemory(&parent.sta.bssid, parentRecord.bssid, 6);
        esp_mesh_set_ap_authmode(parentRecord.authmode);
        if (parentRecord.authmode != WIFI_AUTH_OPEN)
        {
            string temp((myType == MESH_ROOT ? routerPSWD.c_str() : CONFIG_MESH_AP_PASSWD));
            CopyMemory(&parent.sta.password, const_cast<char*>(temp.c_str()), temp.length());
        }
        esp_mesh_set_parent(&parent, (mesh_addr_t *)&parentAssoc.mesh_id, myType, esp_mesh_get_layer());

    }else {
        esp_wifi_scan_stop();
        scanConfig.show_hidden = 1;
        scanConfig.scan_type = WIFI_SCAN_TYPE_PASSIVE;
        esp_wifi_scan_start(&scanConfig, 0);
    }
}

//! \fn     EventHandler
//! \brief  This static function intercepts system events regarding the wifi 
//!         adapter.
//! \params void* and system_event_t.
//! \return esp_error_t code.
//!
void MeshEventHandler(mesh_event_t event)
{
    wifi_scan_config_t scanConfig = {};

    switch (event.id) {
    case MESH_EVENT_STARTED:
        scanConfig.show_hidden = true;
        scanConfig.scan_type   = WIFI_SCAN_TYPE_PASSIVE;

        esp_mesh_set_self_organized(0, 0);
        esp_wifi_scan_stop();
        esp_wifi_scan_start(&scanConfig, 0);

        xEventGroupSetBits(meshEventGroup, meshStartBit);
        break;
    case MESH_EVENT_STOPPED:
        xEventGroupClearBits(meshEventGroup, meshStartBit);
        break;
    case MESH_EVENT_NO_PARENT_FOUND:
        // TODO : stuff
        break;
    case MESH_EVENT_PARENT_CONNECTED:
        if (esp_mesh_is_root())
            tcpip_adapter_dhcpc_start(TCPIP_ADAPTER_IF_STA);
        xEventGroupSetBits(meshEventGroup, meshConnectedBit);
        break;
    case MESH_EVENT_PARENT_DISCONNECTED:
        if (event.info.disconnected.reason == WIFI_REASON_ASSOC_TOOMANY) {
            esp_wifi_scan_stop();
            scanConfig.show_hidden = 1;
            scanConfig.scan_type   = WIFI_SCAN_TYPE_PASSIVE;
            esp_wifi_scan_start(&scanConfig, 0);
        }
        xEventGroupClearBits(meshEventGroup, meshConnectedBit);
        break;
    case MESH_EVENT_SCAN_DONE:
        ScanHandler(event.info.scan_done.number);
        break;
    default:
        break;
    }
}

//! \fn    WifiInit
//! \brief This function initializes the wifi adapter using mostly 
//!        default values.
//!
void WIFI::WifiInit()
{
    wifi_init_config_t wifiCfg = WIFI_INIT_CONFIG_DEFAULT();
    error              err     = {};

    meshEventGroup = xEventGroupCreate();
    macAddr        = bno.GetMac();

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
    mesh_cfg_t  meshCfg = {};
    string      meshPwd(CONFIG_MESH_AP_PASSWD);
    EventBits_t eventBits = {};

    routerSSID = sid;
    routerPSWD = pwd;

    meshCfg.event_cb = &MeshEventHandler;
    meshCfg.channel  = CONFIG_MESH_CHANNEL;
    meshCfg.router.ssid_len = sid.length();
    meshCfg.mesh_ap.max_connection = CONFIG_MESH_AP_CONNECTIONS;

    CopyMemory(reinterpret_cast<byte*>(&meshCfg.mesh_id), macAddr.data(), 6);
    CopyMemory(meshCfg.router.ssid, const_cast<char*>(routerSSID.c_str()), routerSSID.length());
    CopyMemory(meshCfg.router.password, const_cast<char*>(routerPSWD.c_str()), routerPSWD.length());
    CopyMemory(meshCfg.mesh_ap.password, const_cast<char*>(meshPwd.c_str()), meshPwd.length());
    
    esp_mesh_set_ap_authmode(static_cast<wifi_auth_mode_t>(CONFIG_MESH_AP_AUTHMODE));
    esp_mesh_set_config(&meshCfg);
    esp_mesh_start();

    while ((eventBits & meshConnectedBit) == 0)
    {
        cout << ".";
        cout << std::flush;
        eventBits = xEventGroupWaitBits(meshEventGroup, meshConnectedBit, pdFALSE, pdTRUE, ticksToWait);
    }

    cout << "ESP32 connected to SSID!" << endl;
}

//! \fn    WifiDisconnect
//! \brief Disconnects the esp32 from the configured wifi ssid.
//!
void WIFI::WifiDisconnect()
{
    esp_mesh_disconnect();
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

//! \fn     WifiIsMeshEnabled
//! \brief  Indicates whether mesh networking is enabled on the calling
//!         node.
//! \return <bool> true of false.
//!
bool WIFI::MESH::WifiIsMeshEnabled()
{
    return true;
}

//! \fn     WiFiMeshIsRoot
//! \brief  Indicates whether the calling node is root node, or not,
//!         when mesh networking is enabled.
//! \return <bool> true or false.
//!
bool WIFI::MESH::WifiIsRootNode()
{
    return MESH_ROOT == esp_mesh_get_type();
}

