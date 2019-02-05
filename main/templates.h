//! -------------------------------------------------------------------------------------------- //
//! \file  defines.h
//! \brief This header contains various data structures and constants used throughout the library.
//!        It contains enumerations, structs, and constants related to the LSM9DS1 inertia module.
//!
//!
#pragma once
#include "defines.h"


//! \fn     ZeroMemory
//! \brief  ZeroMemory performs a memset to zero memory of any array passed
//!         to the template. Casting the void pointer is automatic.
//! \params <T *destination, size_t len> buffer to zero out, length of buffer.
//!
template <typename T>
void ZeroMemory(T *destination, size_t length)
{
    char *dest = reinterpret_cast<char *>(destination);
    fill(dest, dest + length, '\0');
}

//! \fn     CopyMemory
//! \brief  CopyMemory performs a memory copy operation from the source parameter
//!         to the destination parameter. Casting the void pointer is automatic.
//! \params <T *destination, source, size_t len>destination and source buffers, length.
//!
template<typename T>
void CopyMemory(T *destination, T* source, size_t length)
{
    char *src = reinterpret_cast<char *>(source);
    char *dst = reinterpret_cast<char *>(destination);
    copy(src, src + length, dst);
}

//! \fn     TaskDelay
//! \brief  TaskDelay performs a system delay using the parameter ms, and
//!         simply passes it to the FreeRtos function vTaskDelay. 'ms' is
//!         automatically converted to the correct type.
//! \params <T ms> number of milliseconds.
//!
template <typename T>
void Pause(T ms)
{
    vTaskDelay(static_cast<uint32_t>(ms) / portTICK_PERIOD_MS);
}

//! \fn     CompareTo
//! \brief  CompareTo takes an initializer list, and one item of the same type
//!         as the list, and compares the item to the list to see if it is
//!         included. This removes the need for many comparison statements such
//!         as if (item == 1 || item == 3 || item == 5).
//! \params <T item, initializer_list>
//! \return <bool>
template <typename T>
bool CompareTo(T item, initializer_list<T> list)
{
    for (auto i : list)
    {
        if (i == item)
            return true;
    }
    return false;
}

//! \fn     ReadNVS
//! \brief  ReadNVS takes any of the esp-idf nvs get functions, in the form
//!         nvs_get_u??, as input along with its parameters, and retrieves the
//!         item stored using the key k.
//! \params <function pointer, nvs handle, key string, out buffer>
//! \return <error>
//!
template<typename Func, typename Buff>
error ReadNVS(Func func, nvs_handle h, const char* k, Buff &out)
{
    Buff  result;
    error status = ESP_OK;

    if ((status = func(h, k, &result)) == ESP_OK)
    {
        out = result;
    }else {
        cout << "Error retrieving from NVS: " << status << endl;
    }
    return status;
}
//! \fn     ReadNVS
//! \brief  ReadNVS overload takes the esp-idf nvs get str function, in the form
//!         nvs_get_str, as input along with its parameters, and retrieves the
//!         string stored using the key k.
//! \params <function pointer, nvs handle, key string, out string>
//! \return <error>
//!
template<typename Func>
error ReadNVS(Func func, nvs_handle h, const char* key, string &out)
{
    size_t length;
    char   *str;
    error  status = ESP_OK;

    if ((status = func(h, key, NULL, &length)) == ESP_OK)
    {
        if (length > 1)
        {
            str = new char[length];
            func(h, key, str, &length);
            out = string(str);
            delete str;
        }else {
            cout << "Blank value found using key!" << endl;
            status = ESP_ERR_NVS_INVALID_LENGTH;
        }
    }else {
        cout << "No entry found using key!" << endl;
    }
    return status;
}

