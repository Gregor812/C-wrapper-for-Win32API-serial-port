#pragma once

#include <cinttypes>
#include <Windows.h>

namespace SerialInterfaces
{
    enum class Mode : uint32_t
    {
        Sync = NULL,
        Async = FILE_FLAG_OVERLAPPED
    };
    enum class Baudrate : uint32_t
    {
        BR110 = CBR_110,
        BR300 = CBR_300,
        BR600 = CBR_600,
        BR1200 = CBR_1200,
        BR2400 = CBR_2400,
        BR4800 = CBR_4800,
        BR9600 = CBR_9600,
        BR14400 = CBR_14400,
        BR19200 = CBR_19200,
        BR38400 = CBR_38400,
        BR56000 = CBR_56000,
        BR57600 = CBR_57600,
        BR115200 = CBR_115200,
        BR128000 = CBR_128000,
        BR256000 = CBR_256000
    };

    enum class Parity : uint8_t
    {
        Even = EVENPARITY,
        Mark = MARKPARITY,
        No = NOPARITY,
        Odd = ODDPARITY,
        Space = SPACEPARITY
    };

    enum class DataBits : uint8_t
    {
        Five = 5,
        Six = 6,
        Seven = 7,
        Eight = 8
    };

    enum class StopBits : uint8_t
    {
        One = ONESTOPBIT,
        OneFive = ONE5STOPBITS,
        Two = TWOSTOPBITS
    };

    enum class DtrControl : uint8_t
    {
        Disable = DTR_CONTROL_DISABLE,
        Enable = DTR_CONTROL_ENABLE,
        Handshake = DTR_CONTROL_HANDSHAKE
    };

    enum class RtsControl : uint8_t
    {
        Disable = RTS_CONTROL_DISABLE,
        Enable = RTS_CONTROL_ENABLE,
        Handshake = RTS_CONTROL_HANDSHAKE,
        Toggle = RTS_CONTROL_TOGGLE
    };
}
