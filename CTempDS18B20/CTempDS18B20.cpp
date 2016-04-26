//#define DEBUG_CTempDS18B20

#include "CTempDS18B20.h"

//----------------------------------------------------------------------------
// constructor
//----------------------------------------------------------------------------
CTempDS18B20::CTempDS18B20 (OneWire*    i_poOneWire,
                            byte        i_abyDevAddr[8],
                            EnumResult& o_enResult)
{
  if (!i_poOneWire)
  { o_enResult = EnumResult::Error_ValueInvalid; return;}

  m_poOneWire = i_poOneWire;
  memcpy (m_abyDeviceAddress, i_abyDevAddr, sizeof(m_abyDeviceAddress));
#ifdef DEBUG_CTempDS18B20
  Serial << F("create device ");
  for (int ixCnt = 0; ixCnt < 8; ixCnt++)
    Serial << " " << (m_abyDeviceAddress[ixCnt] > 0xF ? "" : "0") << _HEX(m_abyDeviceAddress[ixCnt]);
  Serial << endl;
#endif

  o_enResult = EnumResult::SUCCESS;
}

//----------------------------------------------------------------------------
// Init
//----------------------------------------------------------------------------
CTempDS18B20::EnumResult CTempDS18B20::Init ()
{
  EnumResult enResult = ResetComm ();
  if (enResult != EnumResult::SUCCESS) return enResult;

  m_poOneWire->select (m_abyDeviceAddress);
  m_poOneWire->write (EnumCmd::CmdReadScratchpad);
  byte abyData[9];
  for (int ixCnt = 0; ixCnt < 9; ixCnt++)
    abyData[ixCnt] = m_poOneWire->read();
  byte byCRC = OneWire::crc8 (abyData, 8);
  if (byCRC != abyData[8]) return EnumResult::Error_WrongCRC;

  enResult = Reset ();
  if (enResult != EnumResult::SUCCESS) return enResult;

  int iResolution = ((abyData[4] >> 5) & 0x3) + 1;
#ifdef DEBUG_CTempDS18B20
  Serial << F("resolution=") << iResolution << endl;
#endif
  m_byTempMultiplier = 1 << iResolution;

  m_bInit = true;
  return enResult;
}

//----------------------------------------------------------------------------
// Reset
//----------------------------------------------------------------------------
CTempDS18B20::EnumResult CTempDS18B20::Reset ()
{
  m_enAction  = EnumAction::ActNone;
  m_byStep    = 0;
  m_tStart    = 0;
  return ResetComm ();
}

//----------------------------------------------------------------------------
// ResetComm
//----------------------------------------------------------------------------
CTempDS18B20::EnumResult CTempDS18B20::ResetComm ()
{
  if (!m_poOneWire) return EnumResult::Error_ObjectNotInitialized;

  byte byResult = m_poOneWire->reset();
  if (!byResult) return EnumResult::Error_NoDevicePresent;

  return EnumResult::SUCCESS;
}

//----------------------------------------------------------------------------
// ReadTemp
//----------------------------------------------------------------------------
CTempDS18B20::EnumResult CTempDS18B20::ReadTemp (bool   i_bWait,
                                                 float& o_fTemp)
{
  if (!m_poOneWire || !m_bInit) return EnumResult::Error_ObjectNotInitialized;
  EnumResult enResult = EnumResult::InProgress;

  if (m_enAction != EnumAction::ActReadTemp
  ||  m_byStep <= 0
  ||  i_bWait)
  {
    m_enAction = EnumAction::ActReadTemp;
    m_byStep = 1;
  }
#ifdef DEBUG_CTempDS18B20
  Serial << F("step=") << m_byStep << F(" wait=") << i_bWait << endl;
#endif

  switch (m_byStep)
  {
  case 1:
    m_poOneWire->select (m_abyDeviceAddress);
    m_byStep++;
    if (!i_bWait) break;

  case 2:
    m_poOneWire->write (EnumCmd::CmdConvertT);
    m_byStep++;
    m_tStart = millis ();
    if (!i_bWait) break;

  case 3:
    for (int iCnt = 1; iCnt <= 1000; iCnt++)
    {
      bool bState = m_poOneWire->read_bit();
      if (bState)
      { m_byStep++; break;}
      if (millis() - m_tStart > READTEMP_TIMEOUT) return EnumResult::Error_Timeout;
      if (!i_bWait) break;
      delay (20);
    }
    if (!i_bWait) break;

  case 4:
    enResult = ResetComm ();
    if (enResult == EnumResult::SUCCESS) enResult = EnumResult::InProgress;
    m_byStep++;
    if (!i_bWait) break;

  case 5:
    m_poOneWire->select (m_abyDeviceAddress);
    m_byStep++;
    if (!i_bWait) break;

  case 6:
    m_poOneWire->write (EnumCmd::CmdReadScratchpad);
    m_byStep++;
    if (!i_bWait) break;

  case 7:
    {
      byte abyData[9];
      for (int ixCnt = 0; ixCnt < 9; ixCnt++)
        abyData[ixCnt] = m_poOneWire->read();
      byte byCRC = OneWire::crc8 (abyData, 8);
      if (byCRC != abyData[8]) return EnumResult::Error_WrongCRC;
      int iTemp = abyData[0] | abyData[1] << 8;
      o_fTemp = (float)iTemp / m_byTempMultiplier;
    #ifdef DEBUG_CTempDS18B20
      Serial << F("temp=") << o_fTemp << endl;
    #endif
    }
    enResult = Reset ();
    break;

  default:
    return EnumResult::Error_InvalidStep;
  }

  return enResult;
}

//----------------------------------------------------------------------------
// SetTempResolution
//----------------------------------------------------------------------------
CTempDS18B20::EnumResult CTempDS18B20::SetTempResolution (int i_iResolution)
{
  if (i_iResolution < 1 || i_iResolution > 4) return EnumResult::Error_ValueOutOfRange;

  return EnumResult::Error_OperationFailed; // writing value to device is not implemented.
  m_byTempMultiplier = 1 << i_iResolution;

  return EnumResult::SUCCESS;
}

//----------------------------------------------------------------------------
// DeviceSearch
//----------------------------------------------------------------------------
CTempDS18B20::EnumResult CTempDS18B20::DeviceSearch (OneWire* i_poOneWire,
                                                     byte     o_aabyDevAddr[][8],
                                                     byte&    o_byDeviceCount)
{
  if (!i_poOneWire) return EnumResult::Error_ValueInvalid;

  byte byDevice = 0;
  bool bFound   = false;
  do
  {
    bFound = i_poOneWire->search (o_aabyDevAddr[byDevice]);
    if (bFound)
    {
      #ifdef DEBUG_CTempDS18B20
      Serial << F("DS18B20 #") << (byDevice+1) << F(", address");
      for (int ixCnt = 0; ixCnt < 8; ixCnt++)
        Serial << " " << (o_aabyDevAddr[byDevice][ixCnt] > 0xF ? "" : "0") << _HEX(o_aabyDevAddr[byDevice][ixCnt]);
      Serial << endl;
      #endif
      byte byCRC = OneWire::crc8 (o_aabyDevAddr[byDevice], 7);
      if (byCRC != o_aabyDevAddr[byDevice][7]) return EnumResult::Error_WrongCRC;
      byDevice++;
    }
  }
  while (bFound);
  o_byDeviceCount = byDevice;

  return EnumResult::SUCCESS;
}
