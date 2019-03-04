//! -------------------------------------------------------------------------------------------- //
//! \file  wifi.cpp
//! \brief This source contains the implementation of the WiFi functions which allow the 
//!        app_main function to connect the esp32 to a designated access point. These functions  
//!        only support station mode.
//!
//!
#include "wifi.h"


//! -------------------------------------------------------------------------------------------- //
//! \brief Globals and constants
//!
egHandle  meshEventGroup    = {};
const int meshStartBit      = BIT0;
const int meshConnectedBit  = BIT1;
const int meshRootGotIpBit  = BIT2;
const int RX_SIZE           = 1500;
const int TX_SIZE           = 1460;
const int MAX_NODES         = 8;
const int ADD_ROOT          = 1;

const TickType_t TICKSTOWAIT = 500 / portTICK_PERIOD_MS;
const uint8_t    MESH_ID[6]  = { 0x7A, 0x69, 0xDE, 0xAD, 0xBE, 0xEF };

string routerSSID;
string routerPSWD;

extern BnoModule bno;


//! -------------------------------------------------------------------------------------------- //
//! \brief Functions section
//!

//! \fn     GetWifiChannel
//! \brief  This function performs a wifi scan, and then cycles through all the scanned access
//!         points, looking for the one with mesh associated data. This will be the softAp of the
//!         root node. When the softAp is found, its channel is returned to caller. If no softAp
//!         is found, channel 1 is returned.
//! \return <byte> the wifi channel.
//!
byte GetWifiChannel()
{
    uint16_t           numAccessPoints = {};
    wifi_scan_config_t scanConfig      = {};

    scanConfig.show_hidden = true;
    scanConfig.ssid        = NULL;
    scanConfig.bssid       = NULL;
    scanConfig.channel     = 0;

    ESP_ERROR_CHECK(esp_wifi_scan_start(&scanConfig, true));
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&numAccessPoints));

    wifi_ap_record_t *apRecords = new wifi_ap_record_t[numAccessPoints];
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&numAccessPoints, apRecords));

    for (int i = 0; i < numAccessPoints; i++)
    {
        string temp((char*)apRecords[i].ssid);
        if (temp.compare(routerSSID) == 0)
        {
            cout << "Found channel!" << endl;
            return apRecords[i].primary;
        }
    }

    return static_cast<byte>(1);
}

//! \fn    ScanHandler
//! \brief This function handles checking each access point found during the
//!        ap scan, and locates the parent node for either root or a leaf node.
//! \param <int> number of access points scanned.
//!
void ScanHandler(int num)
{
    bool parentFound = false;
    int  ieLen       = 0;
    int  myLayer     = {};
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

        if (ieLen == sizeof(assoc) && !bno.IsRoot())
        {
            if (assoc.mesh_type == MESH_ROOT)
            {
                parentFound = true;
                myType      = MESH_LEAF;
                myLayer     = parentAssoc.layer + 1;
                CopyMemory(&parentRecord, &record, sizeof(record));
                CopyMemory(&parentAssoc, &assoc, sizeof(assoc));
                break;
            }
        }else if (ieLen != sizeof(assoc) && bno.IsRoot()) {
            string sid((char *)record.ssid);
            if (sid.compare(routerSSID) == 0)
            {
                parentFound = true;
                myType      = MESH_ROOT;
                myLayer     = MESH_ROOT_LAYER;
                CopyMemory(&parentRecord, &record, sizeof(record));
                break;
            }
        }
    }
    esp_mesh_flush_scan_result();

    if (parentFound)
    {
        parent.sta.channel   = parentRecord.primary;
        parent.sta.bssid_set = 1;
        CopyMemory(&parent.sta.ssid, &parentRecord.ssid, sizeof(parentRecord.ssid));
        CopyMemory(&parent.sta.bssid, parentRecord.bssid, 6);
        
        esp_mesh_set_ap_authmode(parentRecord.authmode);
        if (parentRecord.authmode != WIFI_AUTH_OPEN)
        {
            string temp((myType == MESH_ROOT ? routerPSWD.c_str() : CONFIG_MESH_AP_PASSWD));
            CopyMemory(&parent.sta.password, const_cast<char*>(temp.c_str()), temp.length());
        }
        esp_mesh_set_parent(&parent, (mesh_addr_t *)&parentAssoc.mesh_id, myType, myLayer);
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
    switch (event.id) {
    case MESH_EVENT_STARTED:
        cout << "<MESH_EVENT_STARTED>" << endl;
        xEventGroupSetBits(meshEventGroup, meshStartBit);
        break;
    case MESH_EVENT_STOPPED:
        cout << "<MESH_EVENT_STOPPED>" << endl;
        xEventGroupClearBits(meshEventGroup, meshStartBit);
        break;
    case MESH_EVENT_CHILD_CONNECTED:
        cout << "<MESH_EVENT_CHILD_CONNECTED>" << endl;
        break;
    case MESH_EVENT_ROUTING_TABLE_ADD:
        cout << "<MESH_EVENT_ROUTING_TABLE_ADD>" << endl;
    case MESH_EVENT_NO_PARENT_FOUND:
        cout << "<MESH_EVENT_NO_PARENT_FOUND>" << endl;
        // TODO : stuff
        break;
    case MESH_EVENT_PARENT_CONNECTED:
        cout << "<MESH_EVENT_PARENT_CONNECTED>" << endl;
        if (esp_mesh_is_root())
            tcpip_adapter_dhcpc_start(TCPIP_ADAPTER_IF_STA);
        xEventGroupSetBits(meshEventGroup, meshConnectedBit);
        break;
    case MESH_EVENT_PARENT_DISCONNECTED:
        cout << "<MESH_EVENT_PARENT_DISCONNECTED>" << endl;
        xEventGroupClearBits(meshEventGroup, meshConnectedBit);
        break;
    case MESH_EVENT_ROOT_GOT_IP:
        cout << "<MESH_EVENT_ROOT_GOT_IP>" << endl;
        xEventGroupSetBits(meshEventGroup, meshRootGotIpBit);
    case MESH_EVENT_SCAN_DONE:
        cout << "<MESH_EVENT_SCAN_DONE>" << endl;
        //ScanHandler(event.info.scan_done.number);
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
    esp_mesh_set_max_layer(2);
    esp_mesh_fix_root(true);
    if (bno.IsRoot())
        esp_mesh_set_type(MESH_ROOT);
}

//! \fn    WifiConnect
//! \brief This function connects the esp32 to the configured wifi ssid.
//! \param string ssid, string password.
//!
void WIFI::WifiConnect(string sid, string pwd)
{
    string      meshPwd(CONFIG_MESH_AP_PASSWD);
    mesh_cfg_t  meshCfg   = {};
    EventBits_t eventBits = {};

    routerSSID = sid;
    routerPSWD = pwd;

    /*!< Configure mesh properties */
    meshCfg.event_cb = &MeshEventHandler;
    meshCfg.channel  = (esp_mesh_is_root() ? 1 : GetWifiChannel());
    meshCfg.router.ssid_len = sid.length();
    meshCfg.crypto_funcs    = &g_wifi_default_mesh_crypto_funcs;
    meshCfg.mesh_ap.max_connection = CONFIG_MESH_AP_CONNECTIONS;

    CopyMemory((uint8_t *)&meshCfg.mesh_id, const_cast<byte*>(MESH_ID), 6);
    CopyMemory(meshCfg.router.ssid, const_cast<char*>(routerSSID.c_str()), routerSSID.length());
    CopyMemory(meshCfg.router.password, const_cast<char*>(routerPSWD.c_str()), routerPSWD.length());
    CopyMemory(meshCfg.mesh_ap.password, const_cast<char*>(meshPwd.c_str()), meshPwd.length());
    /*!< Configure mesh properties */
    
    esp_mesh_set_ap_authmode(static_cast<wifi_auth_mode_t>(CONFIG_MESH_AP_AUTHMODE));
    esp_mesh_set_config(&meshCfg);
    esp_mesh_start();

    while ((eventBits & meshConnectedBit) == 0)
    {
        cout << "." << flush;
        eventBits = xEventGroupWaitBits(meshEventGroup, meshConnectedBit, pdFALSE, pdTRUE, TICKSTOWAIT);
    }

    cout << "ESP32 connected to " << (WIFI::MESH::WifiIsMeshEnabled() ? "mesh network!" : "SSID!") << endl;

    if (esp_mesh_is_root())
    {
        cout << "Root waiting for IP address";
        while ((eventBits & meshRootGotIpBit) == 0)
        {
            cout << "." << flush;
            eventBits = xEventGroupWaitBits(meshEventGroup, meshRootGotIpBit, pdFALSE, pdTRUE, TICKSTOWAIT);
        }
    }
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

//! \fn     WifiMeshTxMain
//! \brief  This function handles sending data to other mesh nodes using the esp-idf
//!         mesh API function calls.
//! \param  <string> the data to send.
//!
error WIFI::MESH::WifiMeshTxMain(string data)
{
    byte        txBuf[TX_SIZE]   = {};
    int         flag             = 0;
    int         tableSize        = 0;
    mesh_addr_t table[MAX_NODES + ADD_ROOT];
    mesh_data_t txData;
    error       result;
    
    txData.data  = txBuf;
    txData.size  = data.length();
    txData.tos   = MESH_TOS_P2P;
    txData.proto = (esp_mesh_is_root() ? MESH_PROTO_HTTP : MESH_PROTO_JSON);
    CopyMemory(txData.data, const_cast<char*>(data.c_str()), data.length());

    if (esp_mesh_is_root())
    {
        result = esp_mesh_get_routing_table((mesh_addr_t*)&table, sizeof(table) * 6, &tableSize);
        flag   = MESH_DATA_P2P;
    }

    if (!esp_mesh_is_root())
        tableSize++;
    
    for (int i = 0; i < tableSize; i++)
    {
        if (esp_mesh_is_root() && (i == 0))
            continue;
        if (esp_mesh_is_root())
            result = esp_mesh_send(&table[i], &txData, flag, NULL, 0);
        else
            result = esp_mesh_send(NULL, &txData, flag, NULL, 0);
        
        if (result != ESP_OK)
            return result;
    }
    
    return ESP_OK;
}

//! \fn     WifiMeshRxMain
//! \brief  This function handles receiving data from other mesh nodes using the esp-idf
//!         mesh API function calls.
//! \return <vector<string>> the json array items
//!
strings WIFI::MESH::WifiMeshRxMain(int timeout)
{
    byte        rxBuf[RX_SIZE] = {};
    int         flag           = {};
    strings     response       = {};
    error       result;

    mesh_rx_pending_t pending  = {};
    mesh_addr_t       from;
    mesh_data_t       rxData;

    rxData.data = rxBuf;
    rxData.size = RX_SIZE;

    if (esp_mesh_is_root())
    {
        int routingTableSize = esp_mesh_get_routing_table_size() - 1;
        while (pending.toSelf < routingTableSize)
        {
            esp_mesh_get_rx_pending(&pending);
            Pause(5);
        }
    }
    
    int bound = (esp_mesh_is_root() ? pending.toSelf : 1);
    for (int i = 0; i < bound; i++)
    {
        ZeroMemory(rxData.data, RX_SIZE);
        rxData.size = RX_SIZE;
        result      = esp_mesh_recv(&from, &rxData, timeout, &flag, NULL, 0);
        if (rxData.size == 0)
        {
            string res = "No data!";
            response.push_back(res);
            continue;
        }else if (result != ESP_OK) {
            cout << std::hex << result << std::dec << endl;
            continue;
        }
        string temp(reinterpret_cast<char*>(rxData.data));
        response.push_back(temp);
    }

    return response;
}

