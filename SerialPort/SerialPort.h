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

        void OpenSync(uint16_t _portNumber,
                      Baudrate _baudrate = Baudrate::BR9600,
                      Parity _parity = Parity::No,
                      DataBits _dataBits = DataBits::Eight,
                      StopBits _stopBits = StopBits::One,
                      DtrControl _dtrControl = DtrControl::Enable,
                      RtsControl _rtsControl = RtsControl::Enable) noexcept
        {
            if (m_isOpen == false)
            {
                m_mode = Mode::Sync;
                Open(_portNumber,
                     _baudrate,
                     _parity,
                     _dataBits,
                     _stopBits,
                     _dtrControl,
                     _rtsControl);
            }
        }

        void OpenAsync(uint16_t _portNumber,
                       Baudrate _baudrate = Baudrate::BR9600,
                       Parity _parity = Parity::No,
                       DataBits _dataBits = DataBits::Eight,
                       StopBits _stopBits = StopBits::One,
                       DtrControl _dtrControl = DtrControl::Enable,
                       RtsControl _rtsControl = RtsControl::Enable) noexcept
        {
            if (m_isOpen == false)
            {
                m_mode = Mode::Async;
                Open(_portNumber,
                     _baudrate,
                     _parity,
                     _dataBits,
                     _stopBits,
                     _dtrControl,
                     _rtsControl);
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

                WriteFile(m_portHandle, _data.c_str(), static_cast<DWORD>(_data.length()), &sentBytes, NULL);
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
            std::string empty;
            return empty;
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

        void Open(uint16_t _portNumber,
                  Baudrate _baudrate,
                  Parity _parity,
                  DataBits _dataBits,
                  StopBits _stopBits,
                  DtrControl _dtrControl,
                  RtsControl _rtsControl) noexcept
        {
            m_portNumber = _portNumber;
            m_baudrate = _baudrate;
            m_parity = _parity;
            m_dataBits = _dataBits;
            m_stopBits = _stopBits;
            m_dtrControl = _dtrControl;
            m_rtsControl = _rtsControl;

            /// lock mutex
            m_portHandle = GetHandle();

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

        HANDLE GetHandle() noexcept
        {
            if (m_portHandle == INVALID_HANDLE_VALUE)
            {
                TCHAR tempString[1024] = {};
                wsprintf(tempString, _T("\\\\.\\COM%d"), m_portNumber);

                return CreateFile(tempString,
                                  GENERIC_READ | GENERIC_WRITE,
                                  NULL,
                                  NULL,
                                  OPEN_EXISTING,
                                  static_cast<uint32_t>(m_mode),
                                  NULL);
            }

            return m_portHandle;
        }

        bool ConfigureDCB() noexcept
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
                m_dcb.fDtrControl = static_cast<uint8_t>(m_dtrControl);
                m_dcb.fDummy2 = 0;
                m_dcb.fErrorChar = FALSE;
                m_dcb.fInX = FALSE;
                m_dcb.fNull = FALSE;
                m_dcb.fOutX = FALSE;
                m_dcb.fOutxCtsFlow = FALSE;
                m_dcb.fOutxDsrFlow = FALSE;
                m_dcb.fParity = (m_parity != Parity::No);
                m_dcb.fRtsControl = static_cast<uint8_t>(m_rtsControl);
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
                return false;
            }

            wasNoErrors = SetCommState(m_portHandle, &m_dcb);

            if (wasNoErrors == false)
            {
                Close();
                return false;
            }
            return true;
        }

        bool ConfigureTimeouts() noexcept
        {
            bool wasNoErrors = GetCommTimeouts(m_portHandle, &m_commTimeouts);

            if (wasNoErrors)
            {
                m_commTimeouts.ReadIntervalTimeout = 100;
                m_commTimeouts.ReadTotalTimeoutMultiplier = 2;
                m_commTimeouts.ReadTotalTimeoutConstant = 200;
                m_commTimeouts.WriteTotalTimeoutMultiplier = 2;
                m_commTimeouts.WriteTotalTimeoutConstant = 200;
            }

            wasNoErrors = SetCommTimeouts(m_portHandle, &m_commTimeouts);

            return wasNoErrors;
        }

        /// ===========================================
        /// fields

        HANDLE m_portHandle = INVALID_HANDLE_VALUE;
        bool m_isOpen = false;
        Mode m_mode;

        DCB m_dcb;
        COMMTIMEOUTS m_commTimeouts;
        COMSTAT m_comStat;
        COMMPROP m_commProp;
        COMMCONFIG m_commConfig;

        uint16_t m_portNumber;
        Baudrate m_baudrate;
        Parity m_parity;
        DataBits m_dataBits;
        StopBits m_stopBits;
        DtrControl m_dtrControl;
        RtsControl m_rtsControl;
    };
}