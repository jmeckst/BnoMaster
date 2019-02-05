//! -------------------------------------------------------------------------------------------- //
//! \file  rest.h
//! \brief This header contains the definitions for functions needed to communicate with the 
//!        REST server. Included functions cover the REST basics of CRUD: Create, Retrieve, 
//!        Update, and Delete. Although all are present, only the POST(Create) function is  
//!        implemented at this time.
//!
//!
#pragma once
#include "event.h"

#ifdef CONFIG_ENABLE_MESH_WIFI
#include "../components/MeshWiFi/wifi.h"
#else
#include "../components/SimpleWiFi/wifi.h"
#endif


//! \fn     CreateReading
//! \brief  This function handles POST'ing json data to the REST server.
//! \param  <string> The json data to POST.
//! \return <bool> Success or failure.
//!
rerror CreateReading(string data);

//! \fn     ReadReading
//! \brief  This function handles GET'ing json data from the REST server.
//! \return <bool> Success or failure.
//!
rerror ReadReading();

//! \fn     UpdateReading
//! \brief  This function handles PUT'ing json data onto the REST server 
//!         to update an existing reading.
//! \return <bool> Success or failure.
//!
rerror UpdateReading();

//! \fn     DeleteReading
//! \brief  This function handles DELETE'ing json data from the REST server.
//! \return <bool> Success or failure.
//!
rerror DeleteReading();

//! \fn     FormatDataToJson (overload 1)
//! \brief  This function takes the event objects, reads their x, y, and z 
//!         values, and formats the payload using proper json.
//! \param  <initializer_list> Any number of event objects.
//! \return <string> A properly formated json string.
//!
string FormatDataToJson(initializer_list<SensorEvent*> events);

