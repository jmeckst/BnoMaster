//! -------------------------------------------------------------------------------------------- //
//! \file  wifi.h
//! \brief This header contains the definition of the WiFi functions which allow the app_main
//!        function to connect the esp32 to a designated access point. These functions only 
//!        support station mode.
//!
//!
#pragma once
#include "../../main/bno.h"


namespace WIFI {
    //! \fn    WifiInit
    //! \brief Initializes the wifi adapter.
    //!
    void       WifiInit();

    //! \fn    WifiConnect
    //! \brief Connects the esp32 to the configured wifi ssid.
    //! \param string ssid, string password.
    //!
    void       WifiConnect(string sid, string pwd);

    //! \fn    WifiDisconnect
    //! \brief Disconnects the esp32 from the configured wifi ssid.
    //!
    void       WifiDisconnect();

    //! \fn     WifiGetStatus
    //! \brief  Gets the current status of the wifi connection.
    //! \return wifiStatus enum value.
    //!
    wifiStatus WifiGetStatus();
}

