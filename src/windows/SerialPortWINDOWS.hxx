//============================================================================
//
//   SSSS    tt          lll  lll
//  SS  SS   tt           ll   ll
//  SS     tttttt  eeee   ll   ll   aaaa
//   SSSS    tt   ee  ee  ll   ll      aa
//      SS   tt   eeeeee  ll   ll   aaaaa  --  "An Atari 2600 VCS Emulator"
//  SS  SS   tt   ee      ll   ll  aa  aa
//   SSSS     ttt  eeeee llll llll  aaaaa
//
// Copyright (c) 1995-2019 by Bradford W. Mott, Stephen Anthony
// and the Stella Team
//
// See the file "License.txt" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//============================================================================

#ifndef SERIALPORT_WINDOWS_HXX
#define SERIALPORT_WINDOWS_HXX

#include <windows.h>

#include "SerialPort.hxx"

/**
  Implement reading and writing from a serial port under Windows systems.
*/
class SerialPortWINDOWS : public SerialPort
{
  public:
    SerialPortWINDOWS();
    virtual ~SerialPortWINDOWS();

    /**
      Open the given serial port with the specified attributes.

      @param device  The name of the port
      @return  False on any errors, else true
    */
    bool openPort(const string& device) override;

    /**
      Close a previously opened serial port.
    */
    void closePort() override;

    /**
      Write a byte to the serial port.

      @param data  The byte to write to the port
      @return  True if a byte was written, else false
    */
    bool writeByte(const uInt8* data) override;

  private:
    // Handle to serial port
    HANDLE myHandle;

  private:
    // Following constructors and assignment operators not supported
    SerialPortWINDOWS(const SerialPortWINDOWS&) = delete;
    SerialPortWINDOWS(SerialPortWINDOWS&&) = delete;
    SerialPortWINDOWS& operator=(const SerialPortWINDOWS&) = delete;
    SerialPortWINDOWS& operator=(SerialPortWINDOWS&&) = delete;
};

#endif
