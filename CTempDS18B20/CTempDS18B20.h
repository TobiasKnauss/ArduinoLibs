// DALLAS / MAXIM
// digital temperature sensor DS18B20
//
// ROM commands:
//  SEARCH ROM    [0xF0] : Start device search routine.
//                          Command is followed by the search routine.
//  READ ROM      [0x33] : Read ROM from device (possible if there is only 1 device on the bus).
//                          Command is followed by reading the 8 byte ROM number.
//  MATCH ROM     [0x55] : Select single device.
//                          Command is followed by writing the 8 byte ROM nummber.
//  SKIP ROM      [0xCC] : Select all devices.
//  ALARM SEARCH  [0xEC] : Like SEARCH ROM, but only with the devices in alarm state.
//
// Function commands:
//  CONVERT T         [0x44] : Convert measured temperature into binary value.
//                              Command can be followed by ReadBit to detect the conversion state (0=InProgress, 1=Done).
//  WRITE SCRATCHPAD  [0x4E] : Write scratchpad data: 3 bytes: TH, TL, Config.
//                              Command MUST BE followed by writing 3 bytes. All data MUST be written before next reset, or the data may be corrupted!
//  READ SCRATCHPAD   [0xBE] : Read scratchpad data: 9 bytes: TempLSB, TempMSB, TH, TL, Config, 3x reserved, CRC.
//                              Command is followed by reading 9 bytes. A reset can be sent before all bytes are read.
//  COPY SCRATCHPAD   [0x48] : Copy scratchpad to EEPROM.
//  RECALL E2         [0xB8] : Copy EEPROM to scratchpad (re-load EEPROM data).
//                              Command can be followed by ReadBit to detect the conversion state (0=InProgress, 1=Done).
//  READ POWER SUPPLY [0xB4] : Read power-supply mode. 0=parasite, 1=external.
//                              Command is followed by reading 1 bit.
//
// Memory map:
//  Byte 00: Temperature LSB
//  Byte 01: Temperature MSB
//  Byte 02: TH or User Byte 1
//  Byte 03: TL or User Byte 2
//  Byte 04: Config
//  Byte 05: Reserved 0xFF
//  Byte 06: Reserved
//  Byte 07: Reserved 0x10
//  Byte 08: CRC

#ifndef CTempDS18B20_h
#define CTempDS18B20_h

#include "Arduino.h"
#include "OneWire.h"

#ifdef DEBUG_CTempDS18B20
#include "Streaming.h"
#endif

#define READTEMP_TIMEOUT 2000 // ms

//----------------------------------------------------------------------------
// CTempDS18B20
//----------------------------------------------------------------------------
class CTempDS18B20
{
public:
  //----------------------------------------------------------------------------
  // Enums
  //----------------------------------------------------------------------------
  enum EnumResult
  {
  #include "CommonErrors.h"
  Error_NoDevicePresent,
  Error_WrongCRC,
  };

  //--------------------------------------
  enum EnumCmd
  {
    CmdConvertT         = 0x44,
    CmdWriteScratchpad  = 0x4E,
    CmdReadScratchpad   = 0xBE,
    CmdSaveScratchpad   = 0x48,
    CmdLoadScratchpad   = 0xB8,
  };

  //--------------------------------------
  enum EnumAction
  {
    ActNone,
    ActReadTemp,
  };

  //----------------------------------------------------------------------------
  // constructor
  //----------------------------------------------------------------------------
  CTempDS18B20 (OneWire*    i_poOneWire,
                byte        i_abyDevAddr[8],
                EnumResult& o_enResult);

  //----------------------------------------------------------------------------
  // init
  // description: initialize the class by reading the sensor config.
  //----------------------------------------------------------------------------
  EnumResult Init ();

  //----------------------------------------------------------------------------
  // Reset
  // ResetComm
  // description: Reset the 1-wire bus prior to new communication.
  //   This is necessary if not done before by other functions.
  //   Reset() additionally resets the step variables.
  //----------------------------------------------------------------------------
  EnumResult Reset ();
  EnumResult ResetComm ();

  //----------------------------------------------------------------------------
  // ReadTemp
  // description: Read the temperature value from the sensor.
  //   i_bWait==false: The function returns with after each internal step.
  //   i_bWait==true: The function blocks and returns when finished.
  //   The return value (SUCCESS, InProgress, Error_) tells the operation state.
  //   ResetComm() is executed when finished.
  //----------------------------------------------------------------------------
  EnumResult ReadTemp (bool   i_bWait,
                       float& o_fTemp);

  //----------------------------------------------------------------------------
  // SetTempResolution
  // description: Set the resolution [1..4] of the temperature value.
  //   The resolution directly affects the temperature acquisition time.
  //   TemperatureResolution = 2 ^ -i_iResolution
  //   value : resolution
  //   1 : 0.5°C (1/2) // 2 : 0.25°C // 3 : 0.125°C // 4 : 0.0625°C (1/16)
  //----------------------------------------------------------------------------
  EnumResult SetTempResolution (int i_iResolution);

  //----------------------------------------------------------------------------
  // DeviceSearch
  //----------------------------------------------------------------------------
  static EnumResult DeviceSearch (OneWire* i_poOneWire,
                                  byte     o_aabyDevAddr[][8],
                                  byte&    o_byDeviceCount);

private:
  OneWire*      m_poOneWire;
  byte          m_abyDeviceAddress[8];
  bool          m_bInit     = false;
  EnumAction    m_enAction  = EnumAction::ActNone;
  byte          m_byStep    = 0;
  unsigned long m_tStart    = 0;
  byte          m_byTempMultiplier = 1;
};

#endif
