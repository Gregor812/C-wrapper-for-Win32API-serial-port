#pragma once

#include <Windows.h>
#include <tchar.h>
#include <cinttypes>
#include <string>

#include "SerialPortExceptions.h"
#include "SerialPortUtils.h"

namespace SerialInterfaces
{
    class SerialPort final
    {
    public:
        SerialPort() noexcept = default;

        void Open(uint16_t _portNumber, Baudrate _baudrate, Parity _parity, DataBits _dataBits, StopBits _stopBits) noexcept
        {
            if (m_isOpen == false)
            {
                Open(false, _portNumber, _baudrate, _parity, _dataBits, _stopBits);
            }
        }

        void OpenAsync(uint16_t _portNumber, Baudrate _baudrate, Parity _parity, DataBits _dataBits, StopBits _stopBits) noexcept
        {
            if (m_isOpen == false)
            {
                Open(true, _portNumber, _baudrate, _parity, _dataBits, _stopBits);
            }
        }

        void Close() noexcept
        {
            if (m_isOpen == true || m_portHandle != INVALID_HANDLE_VALUE)
            {
                /// lock mutex
                m_portNumber = 0;
                CloseHandle(m_portHandle);
                m_portHandle = INVALID_HANDLE_VALUE;
                m_isOpen = false;
                /// unlock mutex
            }
        }

        void Send(std::string const& _data) noexcept
        {
            if (m_isOpen)
            {
                DWORD sentBytes = 0;
                
                WriteFile(m_portHandle, _data.c_str(), _data.length(), &sentBytes, NULL);
                Sleep(500);
            }
        }

        std::string Receive() noexcept
        {
            if (m_isOpen)
            {
                DWORD receivedBytes = 0;
                char buffer[128] = {};

                ReadFile(m_portHandle, buffer, sizeof(buffer), &receivedBytes, NULL);

                return std::string(buffer);
            }
        }

        bool IsOpen() const noexcept
        {
            return m_isOpen;
        }

        ~SerialPort() noexcept
        {
            Close();
        }

    private:
        /// =====================================================================================
        /// private methods

        void Open(bool _async, uint16_t _portNumber, Baudrate _baudrate, Parity _parity, DataBits _dataBits, StopBits _stopBits) noexcept
        {
            m_portNumber = _portNumber;
            m_baudrate = _baudrate;
            m_parity = _parity;
            m_dataBits = _dataBits;

            /// lock mutex
            m_portHandle = GetHandle(_async);

            if (m_portHandle == INVALID_HANDLE_VALUE)
            {
                /// TODO: error processing
                /*char errorMessage[1024] = {};
                void *formattedMessageBuffer;
                auto errorCode = GetLastError();

                FormatMessage(
                    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                    NULL,
                    errorCode,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    (LPTSTR)&formattedMessageBuffer,
                    0,
                    NULL
                );
                wsprintf(tempString, _T("Could not open serial port COM%d\nFailed with error %d:\n%s\n"), portNumber_, errorCode, formattedMessageBuffer);
                WideCharToMultiByte(1, NULL, tempString, -1, errorMessage, sizeof(errorMessage), NULL, NULL);
                LocalFree(formattedMessageBuffer);
                throw CannotOpenSerialPortException(errorMessage);*/

                m_isOpen = false;
                return;
            }

            ConfigureDCB();
            ConfigureTimeouts();

            m_isOpen = true;
            /// unlock mutex
        }

        HANDLE GetHandle(bool _async) noexcept
        {
            if (m_portHandle == INVALID_HANDLE_VALUE)
            {
                TCHAR tempString[1024] = {};
                wsprintf(tempString, _T("\\\\.\\COM%d"), m_portNumber);

                DWORD fileFlag = _async ? FILE_FLAG_OVERLAPPED : NULL;

                return CreateFile(tempString,
                                  GENERIC_READ | GENERIC_WRITE,
                                  NULL,
                                  NULL,
                                  OPEN_EXISTING,
                                  fileFlag,
                                  NULL);
            }

            return m_portHandle;
        }

        void ConfigureDCB() noexcept
        {
            memset(&m_dcb, 0, sizeof(DCB));
            m_dcb.DCBlength = sizeof(DCB);

            bool wasNoErrors = GetCommState(m_portHandle, &m_dcb);

            if (wasNoErrors == true)
            {
                m_dcb.BaudRate = static_cast<uint32_t>(m_baudrate);
                m_dcb.ByteSize = static_cast<uint8_t>(m_dataBits);
                m_dcb.EofChar = '\0';
                m_dcb.ErrorChar = '\0';
                m_dcb.EvtChar = '\0';
                m_dcb.fAbortOnError = FALSE;
                m_dcb.fBinary = TRUE;
                m_dcb.fDsrSensitivity = FALSE;
                m_dcb.fDtrControl = DTR_CONTROL_DISABLE;
                m_dcb.fDummy2 = 0;
                m_dcb.fErrorChar = FALSE;
                m_dcb.fInX = FALSE;
                m_dcb.fNull = FALSE;
                m_dcb.fOutX = FALSE;
                m_dcb.fOutxCtsFlow = FALSE;
                m_dcb.fOutxDsrFlow = FALSE;
                m_dcb.fParity = (m_parity != Parity::No);
                m_dcb.fRtsControl = RTS_CONTROL_ENABLE;
                m_dcb.fTXContinueOnXoff = TRUE;
                m_dcb.Parity = static_cast<uint8_t>(m_parity);
                m_dcb.StopBits = static_cast<uint8_t>(m_stopBits);
                m_dcb.wReserved = 0;
                m_dcb.wReserved1 = 0;
                m_dcb.XoffChar = '\0';
                m_dcb.XoffLim = 0;
                m_dcb.XonChar = '\0';
                m_dcb.XonLim = 0;
            }
            else
            {
                Close();
                return;
            }

            wasNoErrors = SetCommState(m_portHandle, &m_dcb);

            if (wasNoErrors == false)
                Close();
        }

        void ConfigureTimeouts() noexcept
        {

        }

        /// ===========================================
        /// fields

        HANDLE m_portHandle = INVALID_HANDLE_VALUE;
        bool m_isOpen = false;
        DCB m_dcb;
        COMMTIMEOUTS m_commTimeouts;

        uint16_t m_portNumber = 0;
        Baudrate m_baudrate = Baudrate::BR9600;
        Parity m_parity = Parity::No;
        DataBits m_dataBits = DataBits::Eight;
        StopBits m_stopBits = StopBits::One;
    };
}