//! -------------------------------------------------------------------------------------------- //
//! \file  rest.cpp
//! \brief This source contains the implementation of functions needed to communicate with the 
//!        REST server. Included functions cover the REST basics of CRUD: Create, Retrieve, 
//!        Update, and Delete. Although all are present, only the POST(Create) function is  
//!        implemented at this time.
//!
//!
#include "rest.h"


extern string SRV;
extern string PORT;

//! \fn     FormatDataToJson
//! \brief  This function takes the event objects from leaf nodes, reads their x, y,  
//!         and z values, and formats them using proper json. This override only returns
//!         the json array item, not the entire array.
//! \param  <initializer_list> Any number of event objects.
//! \return <vector<string>> A vector of strings.
//!
strings FormatDataToJson(eventList events)
{
    ostringstream data;
    strings       result = {};
    int i = 1;

    for (auto e : events)
    {
        data << "\t\t{";
        data << "\"type\":\"" << e->GetName()         << "\", ";
        data << "\"body\":\"" << e->GetLocation()     << "\", ";
        data << "\"ticks\":\"" << esp_timer_get_time() << "\", ";
        if (e->GetObject().IsQuaternion())
            data << "\"W\":\"" << e->GetObject().GetEventW() << "\", ";
        data << "\"X\":\"" << e->GetObject().GetEventX() << "\", ";
        data << "\"Y\":\"" << e->GetObject().GetEventY() << "\", ";
        data << "\"Z\":\"" << e->GetObject().GetEventZ() << "\"";
        data << "}";
        if (i < events.size())
            data << ",\n";
        i++;
        result.push_back(data.str());
    }
    return result;
}

//! \fn     FormatDataToJson
//! \brief  This function takes the event objects, reads their x, y, and z 
//!         values, and formats the payload using proper json. In addition,
//!         extra json array items can be passed from mesh leaf-nodes in the
//!         form of a vector<string>.
//! \param  <initializer_list> Any number of event objects.
//!         <vector<string>>   extra json array items.
//! \return <string> A properly formated json string.
//!
string FormatDataToJson(eventList events, strings extra)
{
    ostringstream data;
    int i = 1;

    data << "{\n\t\"things\":[\n";

    for (auto e : events)
    {
        data << "\t\t{";
        data << "\"type\":\"" << e->GetName()         << "\", ";
        data << "\"body\":\"" << e->GetLocation()     << "\", ";
        data << "\"ticks\":\"" << esp_timer_get_time() << "\", ";
        if (e->GetObject().IsQuaternion())
            data << "\"W\":\"" << e->GetObject().GetEventW() << "\", ";
        data << "\"X\":\"" << e->GetObject().GetEventX() << "\", ";
        data << "\"Y\":\"" << e->GetObject().GetEventY() << "\", ";
        data << "\"Z\":\"" << e->GetObject().GetEventZ() << "\"";
        data << "}";
        if (i < events.size())
            data << ",\n";
        i++;
    }

    if (!extra.empty())
    {
        for (auto e : extra)
        {
            data << ",\n";
            data << e.data();
        }
    }

    data << "\n\t]\n}";

    return data.str();
}

//! \fn     BuildPostHeaders
//! \brief  This function puts together the appropriate headers necessary 
//!         for a POST request and returns them in a string.
//! \param  <size_t> Length for the Content-Length field.
//! \return <string> Completed POST headers.
//!
string BuildPostHeaders(int len)
{
    string tempHost = HOST;
    string tempLeng = LENG;

    tempHost.insert(6, SRV + ":" + PORT);
    
    ostringstream tmp;
    tmp << len;
    tempLeng.insert(16, tmp.str());

    return POST + tempHost + USER + TYPE + tempLeng + CONN + NEWL;
}

//! \fn     ExtractHttpFieldValue
//! \brief  This function extracts an http field from an http response, then
//!         identifies the value, and returns the value to the calling function.
//!         If the requested field wasn't in the response, a blank string is
//!         returned.
//! \params <string, string>field and http response.
//! \return <string>field value, or blank string.
//!
string ExtractHttpFieldValue(string field, string response)
{
    int index1 = response.find(field);
    int index2 = {};

    if (index1 == string::npos)
        return "";
    
    index1 += field.length() + 2;
    index2  = response.find("\r\n", index1);

    return response.substr(index1, index2 - index1);
}

//! \fn     SendToServer
//! \brief  This function handles communication with the server. If Mesh Wifi is
//!         turned on, esp_mesh_send() is used, otherwise a standard socket. This
//!         function abstracts those details from the CRUD functions so they can
//!         perform any HTTP request without needing to worry about the method of
//!         communication.
//! \params <string> headers
//!         <string> data
//!         <string> output
//! \return <error>  esp_err_t
//!
void SendToServer(string headers, string data, string &output)
{
    char recvBuf[500] = {};
    int  sock         = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr;
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(atoi(PORT.c_str()));
    addr.sin_addr.s_addr = inet_addr(SRV.c_str());

    if (connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr)) != 0)
    {
        output = "7";
        goto cleanup;
    }
    cout << "Connected to socket!" << endl;
    if (send(sock, headers.c_str(), headers.length(), 0) < 0)
    {
        output = "8";
        goto cleanup;
    }else {
        send(sock, data.c_str(), data.length(), 0);
        cout << "Wrote headers and data!" << endl;
    }

    if (recv(sock, recvBuf, sizeof(recvBuf) - 1, 0) < 0)
    {
        output = "9";
        goto cleanup;
    }
    cout << "Read response!" << endl;
    output = ExtractHttpFieldValue("Response", string(recvBuf));

cleanup:
    shutdown(sock, 0);
    close(sock);
}

//! \fn     CreateReading
//! \brief  This function handles POST'ing json data to the REST server.
//! \param  <string> The json data to POST.
//! \return <bool> Success or failure.
//!
rerror CreateReading(eventList events)
{
    string  response = {};
    strings meshData = {};
    rerror  result   = REST_OK;

    if (WIFI::WifiGetStatus() == WIFI_STATUS_DISCONNECTED)
        return REST_NO_WIFI;
    
    /*!< Root Section */
    if (!WIFI::MESH::WifiIsMeshEnabled() || WIFI::MESH::WifiIsRootNode())
    {
        meshData    = WIFI::MESH::WifiMeshRxMain(0);                        /*!< First, Rx the leaf node data */
        string data = FormatDataToJson(events, meshData);
        string post = BuildPostHeaders(data.length());

        SendToServer(post, data, response);                                 /*!< Second, Tx to server */
        if (!response.empty())
            result = static_cast<rerror>(atoi(response.c_str()));

        WIFI::MESH::WifiMeshTxMain(response);                               /*!< Last, Tx the response to leaf nodes */

    /*!< Leaf node section */
    }else {
        strings data = {};
        data = FormatDataToJson(events);

        for (auto d : data)                                                 /*!< First, Tx the data */
            WIFI::MESH::WifiMeshTxMain(d);

        meshData = WIFI::MESH::WifiMeshRxMain(portMAX_DELAY);               /*!< Second, Rx the response */

        response = meshData.front();                                        /*!< The response is the first vector item */
        if (!response.empty() && response.find("No data:") == string::npos)
            result = static_cast<rerror>(atoi(response.c_str()));
    }

    return result;
}

//! \fn     ReadReading
//! \brief  This function handles GET'ing json data from the REST server.
//! \return <bool> Success or failure.
//!
rerror ReadReading()
{
    return REST_OK;
}

//! \fn     UpdateReading
//! \brief  This function handles PUT'ing json data onto the REST server 
//!         to update an existing reading.
//! \return <bool> Success or failure.
//!
rerror UpdateReading()
{
    return REST_OK;
}

//! \fn     DeleteReading
//! \brief  This function handles DELETE'ing json data from the REST server.
//! \return <bool> Success or failure.
//!
rerror DeleteReading()
{
    return REST_OK;
}

