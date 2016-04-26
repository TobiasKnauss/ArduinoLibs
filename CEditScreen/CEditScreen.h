#ifndef CEditScreen_h
#define CEditScreen_h

#define EDITDISABLED 0xFFF

#include "Arduino.h"
#include <Streaming.h>
#include <LiquidCrystal_I2C.h>

class CEditScreen
{
public:
  //----------------------------------------------------------------------------
  enum EnumDataType
  {
    BOOL,
    BYTE,
    INT,
    UINT,
    LONG,
    ULONG,
    FLOAT,
    DOUBLE,  // notice: on Arduino Uno, Double = 4 byte (== float)!
    BYTES,
  };

  //----------------------------------------
  enum EnumAction
  {
    None,
    EditStart,
    EditFinish,
    Clear,
    Select,
    Increment,
    Decrement,
  };

  //----------------------------------------------------------------------------
  CEditScreen(LiquidCrystal_I2C*  i_poDisplay,
              byte                i_bySizeX,
              bool                i_bInverseNavigation);

  //----------------------------------------------------------------------------
  // Show
  // i_pbyValue: pointer to the 1st byte of the data.
  // i_byLenNumber   (e.g. 5)
  // i_byLenDecimals (e.g. 3; only used at FLOAT, DOUBLE)
  //                             place:    54321.123
  //   (INT, LONG):      value 12:          '   12'---
  //                     value 456789:      '56789'---  (number is truncated)
  //   (FLOAT, DOUBLE):  value 2.4:         '    2.4  '
  //                     value 234567.2468: '34567.247' (truncated, rounded)
  //----------------------------------------------------------------------------
  void  Show (const __FlashStringHelper* i_poText,
              EnumDataType  i_enDataType,
              byte*   i_pbyValue,
              double  i_dLimit1,
              double  i_dLimit2,
              byte    i_byLenNumber,
              byte    i_byLenDecimals,
              bool    i_bMenuAtTop);

  void  Update (EnumAction i_enAction);

  bool  IsDataExist ()      { return m_bIsDataExist; }
  bool  IsEditActive ()     { return m_iCursorPos < EDITDISABLED; }
  bool  IsDataChanged ()    { return m_bIsDataChanged; }
  void  ResetDataChanged () { m_bIsDataChanged = false; }

  //----------------------------------------
private:
  LiquidCrystal_I2C*  m_poDisplay;
  byte    m_bySizeX;
  bool    m_bInverseNavigation;
  int     m_iCursorPosAbort;
  int     m_iCursorPosSave;

  byte*   m_pbyValue;
  byte*   m_pbyValueOrig;
  double  m_dLowerLimit = 0;
  double  m_dUpperLimit = 0;

  EnumDataType m_enDataType;
  byte    m_byDataSize        = 0;
  byte    m_byStorageSize     = 0;
  byte    m_byTextLenNumber   = 0;
  byte    m_byTextLenDecimals = 0;
  byte    m_byTextLenTotal    = 0;
  byte    m_byRowMenu         = 0;

  bool    m_bIsDataExist    = false;
  bool    m_bIsDataChanged  = false;
  bool    m_bSelected       = false;
  int     m_iCursorPos      = EDITDISABLED;

  //----------------------------------------
private:
  void ShowValue ();
  void LoadValue ();
  void SaveValue ();
  void ChangeValue (bool i_bDirection);
};

#endif
