//! -------------------------------------------------------------------------------------------- //
//! \file  includes.h
//! \brief This header contains various include directives common to the library 
//!        that may be needed in several files.
//!
//!
#pragma once

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "driver/uart.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_mesh.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "lwip/sockets.h"

using std::array;
using std::begin;
using std::copy;
using std::cout;
using std::end;
using std::endl;
using std::equal;
using std::fill;
using std::flush;
using std::initializer_list;
using std::map;
using std::ostringstream;
using std::string;
using std::vector;

