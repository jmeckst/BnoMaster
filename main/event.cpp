//! -------------------------------------------------------------------------------------------- //
//! \file  event.cpp
//! \brief This source contains the implementation of the SensorEvent class which is an event 
//!        object representing a sensor event. It allows the LsmModule to store information 
//!        about an event and be able to retrieve it later by calling a "GetEvent" function.
//!
//!
#include "event.h"


//! \memberof SensorEvent
//! \brief    This is the constructor that takes a quaternion pointer for 
//!           a sensor reading.
//! \params   Quaternion q, string n.
//!
SensorEvent::SensorEvent(Quaternion* q, string n, devLocation l)
{
    name = n;
    obj  = q;
    loc  = l;
}

//! \memberof SensorEvent
//! \brief    This is the copy constructor that provides shallow copy 
//!           of all data members in the object.
//! \params   SensorEvent object reference.
//!
SensorEvent::SensorEvent(const SensorEvent& s)
{
    name        = s.name;

    if (s.obj != nullptr)
        obj = s.obj;
}

//! \memberof SensorEvent
//! \brief    This is the destructor for the event class. Its main 
//!           purpose is to delete the object pointers stored in the 
//!           union to ensure proper memory management.
//!
SensorEvent::~SensorEvent()
{
    if (obj != nullptr)
        delete obj;
}

