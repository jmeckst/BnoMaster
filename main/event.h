//! -------------------------------------------------------------------------------------------- //
//! \file  event.h
//! \brief This header contains the definition of the SensorEvent class which is an event object 
//!        representing a sensor event. It allows the LsmModule to store information about an 
//!        event and be able to retrieve it later by calling a "GetEvent" function.
//!
//!
#pragma once
#include "defines.h"


//! \class SensorEvent event.h
//! \brief The sensor event class that encapsulates an event and
//!        the associated data.
//!
class SensorEvent
{
public:
    SensorEvent(){}
    SensorEvent(Quaternion* q, string n, devLocation l);

    SensorEvent(const SensorEvent& s);
    ~SensorEvent();

    Quaternion& GetObject()   { return *obj; }
    string      GetName()     { return name; }
    string      GetLocation() { return locationToString.at(loc); }

    void        SetObj(Quaternion *o) { obj = o; }
    void        SetName(string n)     { name = n; }
    void        SetLocation(byte l)   { loc = static_cast<devLocation>(l); }

private:
    Quaternion* obj;                    /*!< Sensor object, either vector or quaternion */
    string      name;                   /*!< Json name for post'ing to server */
    devLocation loc;
};

