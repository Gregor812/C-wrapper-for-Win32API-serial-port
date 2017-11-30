#include <iostream>
#include <memory>

#include "SerialPort.h"

using namespace SerialInterfaces;

int main()
{
    /// start test with gsm-modem iRZ TU31
    auto port = std::shared_ptr<SerialPort>(new SerialPort());
    port->OpenSync(2, Baudrate::BR115200);

    port->Send("AT+CGMM\r");
    auto received = port->Receive();

    std::cout << received << std::endl;
    /// stop test

    std::string textToSend;
    char currentChar = 0;

    while (true)
    {
        if (!std::cin.get(currentChar))
            break;

        if (currentChar == '\n')
            continue;

        if (currentChar == '\\')
        {
            if (!std::cin.get(currentChar))
                break;

            if (currentChar == 'n')
            {
                textToSend += '\n';
                continue;
            }
            else if (currentChar == 'r')
            {
                textToSend += '\r';
                port->Send(textToSend);
                textToSend.clear();
                received = port->Receive();
                std::cout << received;
            }
            else
            {
                textToSend += '\\';
                textToSend += currentChar;
            }

            continue;
        }

        textToSend += currentChar;

        if ((textToSend.compare("exit") == 0) ||
            (textToSend.compare("Exit") == 0))
            break;
    }

    return 0;
}