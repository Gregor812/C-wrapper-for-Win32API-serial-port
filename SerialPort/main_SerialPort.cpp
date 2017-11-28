#include <iostream>
#include <memory>

#include "SerialPort.h"

using namespace SerialInterfaces;

int main()
{
    auto port = std::shared_ptr<SerialPort>(new SerialPort());
    port->Open(2, Baudrate::BR115200, Parity::No, DataBits::Eight, StopBits::One);

    port->Send("AT+CGMM\r");
    auto received = port->Receive();

    std::cout << received << std::endl;


    return 0;
}