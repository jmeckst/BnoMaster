//! -------------------------------------------------------------------------------------------- //
//! \file  sparkfun.h
//! \brief This header contains the helper functions used for interacting with the esp32 esp-idf
//!        framework. Modules are separated by namespaces.
//!
//!
#pragma once
#include "defines.h"
#include "templates.h"


namespace UART {
    void InitUART(uport uaPort, line txPin, line rxPin);
}

namespace NVS {
    error OpenNVSPartition(string partName, string nsName);
    error ReadDeviceConfig(byte &loc, word &id, TESTINFO &ti);
    error ReadNetConfig(string &ssid, string &pwd, string &srv, string &port);
}

