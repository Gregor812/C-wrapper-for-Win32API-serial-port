#pragma once

#include <exception>

namespace SerialInterfaces
{
    class CannotOpenSerialPortException : public std::exception
    {
        using std::exception::exception;
    };
}