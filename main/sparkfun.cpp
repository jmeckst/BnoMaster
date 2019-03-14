//! -------------------------------------------------------------------------------------------- //
//! \file  sparkfun.cpp
//! \brief This source file contains the helper functions used for interacting with the esp32 
//!        esp-idf framework. Modules are separated by namespaces.
//!
//!
#include "sparkfun.h"


nvs_handle handle;

//! \fn     InitUART
//! \brief  InitUART sets up the configuration of the UART driver run by the esp32 by 
//!         providing gpio pins and other settings, includes tx, rx, and port.
//! \return <error> esp error code.
//!
void UART::InitUART(uport uaPort, line txPin, line rxPin)
{
    uart_config_t config = {};

    config.baud_rate = 115200;
    config.data_bits = UART_DATA_8_BITS;
    config.parity    = UART_PARITY_DISABLE;
    config.stop_bits = UART_STOP_BITS_1;
    config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;

    uart_param_config(uaPort, &config);
    uart_set_pin(uaPort, txPin, rxPin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(uaPort, 128 * 2, 0, 0, NULL, 0);
}

//! \fn     OpenPartition
//! \brief  This function handles initializing and opening an NVS partition using its
//!         partition name, and a namespace name.
//! \params <string partition, namespace>
//! \return <nvs_handle>
//!
error NVS::OpenNVSPartition(string partName, string nsName)
{
    error status;

    /*!< Initialize and open the flash partition */
    status = nvs_flash_init_partition(partName.c_str());
    if (status == ESP_ERR_NVS_NO_FREE_PAGES)
    {
        nvs_flash_erase_partition(partName.c_str());
        status = nvs_flash_init_partition(partName.c_str());
    }
    status = nvs_open_from_partition(
        partName.c_str(), 
        nsName.c_str(), 
        NVS_READONLY, 
        &handle
    );
    
    return status;
}

//! \fn     ReadDeviceConfig
//! \brief  ReadDeviceConfig opens the custom partition that stores device configuration
//!         information. This information includes device id, and position information.
//! \return <error> esp error code.
//!
error NVS::ReadDeviceConfig(byte &loc, word &id, string &test)
{
    error status;

    /*!< Read device location using nvs_get_U8 */
    if ((status = ReadNVS(&nvs_get_u8, handle, "deviceLoc", loc)) != ESP_OK)
        return status;
    else
        cout << "Device location retrieved!" << endl;

    /*!< Read device id using nvs_get_u16 */
    if ((status = ReadNVS(&nvs_get_u16, handle, "deviceId", id)) != ESP_OK)
        return status;
    else
        cout << "Device id retrieved!" << endl;

    /*!< Check for device test and iterations */
    if ((status = ReadNVS(&nvs_get_str, handle, "test", test)) != ESP_OK)
        return status;
    else
        cout << "Test retrieved!" << endl;
    return ESP_OK;
}

error NVS::ReadNetConfig(string &ssid, string &pwd, string &srv, string &port)
{
    error status;

    /*!< Read wifi ssid using nvs_get_str */
    if ((status = ReadNVS(&nvs_get_str, handle, "ssid", ssid)) != ESP_OK)
        return status;
    else
        cout << "WiFi ssid retrieved!" << endl;
    
    /*!< Read wifi password using nvs_get_str */
    if ((status = ReadNVS(&nvs_get_str, handle, "pwd", pwd)) != ESP_OK)
        return status;
    else
        cout << "WiFi password retrieved!" << endl;
    
    /*!< Read server ip using nvs_get_str */
    if ((status = ReadNVS(&nvs_get_str, handle, "srv", srv)) != ESP_OK)
        return status;
    else
        cout << "Server ip retrieved!" << endl;
    
    /*!< Read server port using nvs_get_str */
    if ((status = ReadNVS(&nvs_get_str, handle, "port", port)) != ESP_OK)
        return status;
    else
        cout << "Server port retrieved!" << endl;
    
    return status;
}

