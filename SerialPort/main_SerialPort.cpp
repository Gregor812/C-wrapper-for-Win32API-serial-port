#include <iostream>
#include <memory>

#include "SerialPort.h"

using namespace SerialInterfaces;

int main()
{
    auto port = std::shared_ptr<SerialPort>(new SerialPort());
    port->Open(15, Baudrate::BR115200, Parity::No, DataBits::Eight, StopBits::One);

    return 0;
}