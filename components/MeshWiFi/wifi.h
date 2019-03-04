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
    //! \fn     WifiInit
    //! \brief  Initializes the wifi adapter.
    //!
    void        WifiInit();

    //! \fn     WifiConnect
    //! \brief  Connects the esp32 to the configured wifi ssid.
    //! \param  string ssid, string password.
    //!
    void        WifiConnect(string sid, string pwd);

    //! \fn     WifiDisconnect
    //! \brief  Disconnects the esp32 from the configured wifi ssid.
    //!
    void        WifiDisconnect();

    //! \fn     WifiGetStatus
    //! \brief  Gets the current status of the wifi connection.
    //! \return wifiStatus enum value.
    //!
    wifiStatus  WifiGetStatus();

    namespace MESH {
        //! \fn     WifiIsMeshEnabled
        //! \brief  Indicates whether mesh networking is enabled on the calling
        //!         node.
        //! \return <bool> true of false.
        //!
        bool    WifiIsMeshEnabled();

        //! \fn     WiFiMeshIsRoot
        //! \brief  Indicates whether the calling node is root node, or not,
        //!         when mesh networking is enabled.
        //! \return <bool> true or false.
        //!
        bool    WifiIsRootNode();

        //! \fn     WifiMeshTxMain
        //! \brief  This function handles sending data to other mesh nodes using the esp-idf
        //!         mesh API function calls.
        //! \param  <string> the data to send.
        //!
        error   WifiMeshTxMain(string data);

        //! \fn     WifiMeshRxMain
        //! \brief  This function handles receiving data from other mesh nodes using the esp-idf
        //!         mesh API function calls.
        //! \return <vector<string>> the json array items
        //!
        strings WifiMeshRxMain(int timeout);
    }
}

