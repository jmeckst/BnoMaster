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

//! \fn     BuildPostHeaders
//! \brief  This function puts together the appropriate headers necessary 
//!         for a POST request and returns them in a string.
//! \param  <size_t> Length for the Content-Length field.
//! \return <string> Completed POST headers.
//!
string BuildPostHeaders(size_t len)
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
    int index1  = response.find(field);
    int index2 = {};

    if (index1 == string::npos)
        return "";
    
    index1 += field.length() + 2;
    index2  = response.find("\r\n", index1);

    return response.substr(index1, index2 - index1);
}

//! \fn     CreateReading
//! \brief  This function handles POST'ing json data to the REST server.
//! \param  <string> The json data to POST.
//! \return <bool> Success or failure.
//!
rerror CreateReading(string data)
{
    if (WIFI::WifiGetStatus() == WIFI_STATUS_CONNECTED)
    {
        string request      = {};
        string post         = BuildPostHeaders(data.length());
        char   recvBuf[500] = {};
        int    sock         = socket(AF_INET, SOCK_STREAM, 0);
        rerror result       = REST_OK;

        sockaddr_in addr;
        addr.sin_family      = AF_INET;
        addr.sin_port        = htons(atoi(PORT.c_str()));
        addr.sin_addr.s_addr = inet_addr(SRV.c_str());

        if (connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr)) != 0)
        {
            close(sock);
            return REST_CONNECT_FAIL;
        }

        if (write(sock, post.c_str(), post.length()) < 0)
        {
            close(sock);
            return REST_WRITE_FAIL;
        }else {
            write(sock, data.c_str(), data.length());
        }

        read(sock, recvBuf, sizeof(recvBuf) - 1);

        request = ExtractHttpFieldValue("Response", string(recvBuf));
        if (!request.empty())
            result = static_cast<rerror>(atoi(request.c_str()));

        close(sock);

        return result;
    }
    return REST_NO_WIFI;
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

//! \fn     FormatDataToJson
//! \brief  This function takes the event objects, reads their x, y, and z 
//!         values, and formats the payload using proper json.
//! \param  <initializer_list> Any number of event objects.
//! \return <string> A properly formated json string.
//!
string FormatDataToJson(initializer_list<SensorEvent*> events)
{
    ostringstream data;
    size_t        i = 1;

    data << "{\n\t\"things\":[\n";

    for (auto e : events)
    {
        data << "\t\t{";
        data << "\"type\":\"" << e->GetName()         << "\", ";
        data << "\"body\":\"" << e->GetLocation()     << "\", ";
        data << "\"time\":\"" << esp_timer_get_time() << "\", ";
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
    data << "\n\t]\n}";

    return data.str();
}

