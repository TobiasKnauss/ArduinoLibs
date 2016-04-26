#include "CEditScreen.h"

//----------------------------------------
// constructor
//----------------------------------------
CEditScreen::CEditScreen (LiquidCrystal_I2C* i_poDisplay,
                          byte  i_bySizeX,
                          bool  i_bInverseNavigation)
{
  m_poDisplay = i_poDisplay;
  m_bySizeX   = i_bySizeX;
  m_bInverseNavigation = i_bInverseNavigation;
  
  m_iCursorPosAbort = 0x100 + m_bySizeX - 2;
  m_iCursorPosSave  = 0x100 + m_bySizeX - 1;
}

//----------------------------------------
// Show
//----------------------------------------
void CEditScreen::Show (const __FlashStringHelper*  i_poText,
                        EnumDataType  i_enDataType,
                        byte*   i_pbyValue,
                        double  i_dLimit1,
                        double  i_dLimit2,
                        byte    i_byLenNumber,
                        byte    i_byLenDecimals,
                        bool    i_bMenuAtTop)
{
  if (!m_poDisplay) return;
  Serial << F("Show ") << i_poText << endl;

  m_iCursorPos      = EDITDISABLED;
  m_bSelected       = false;
  m_bIsDataExist    = true;
  m_bIsDataChanged  = false;
  
  m_pbyValueOrig    = i_pbyValue;
  m_dLowerLimit     = i_dLimit1 < i_dLimit2 ? i_dLimit1 : i_dLimit2;
  m_dUpperLimit     = i_dLimit1 > i_dLimit2 ? i_dLimit1 : i_dLimit2;
  
  m_byRowMenu       = (i_bMenuAtTop ? 0 : 1);
  m_enDataType      = i_enDataType;
  
  if (m_dLowerLimit < 0) i_byLenNumber++;
  if (i_byLenNumber > m_bySizeX) i_byLenNumber = m_bySizeX;
  if (i_byLenNumber < 1) i_byLenNumber = 1;
  m_byTextLenNumber = i_byLenNumber;

  byte byMaxTextLenDecimals = m_bySizeX - m_byTextLenNumber;
  switch (m_enDataType)
  {
  case EnumDataType::BOOL:
    m_byDataSize = 1;
    m_byTextLenNumber = 1;
    byMaxTextLenDecimals = 0;
    m_dLowerLimit = m_dUpperLimit = 0;
    break;
  case EnumDataType::BYTE:
    m_byDataSize = 1;
    byMaxTextLenDecimals = 0;
    break;
  case EnumDataType::INT:
  case EnumDataType::UINT:
    m_byDataSize = 2;
    byMaxTextLenDecimals = 0;
    break;
  case EnumDataType::LONG:
  case EnumDataType::ULONG:
    m_byDataSize = 4;
    byMaxTextLenDecimals = 0;
    break;
  case EnumDataType::FLOAT:
    m_byDataSize = 4;
    break;
  case EnumDataType::DOUBLE:
    m_byDataSize = 8;  // on Arduino Uno, only 4 bytes (== float)
    break;
  case EnumDataType::BYTES:
    if (i_byLenNumber * 2 > m_bySizeX) i_byLenNumber = m_bySizeX / 2;
    m_byDataSize = i_byLenNumber;
    m_byTextLenNumber = i_byLenNumber * 2;
    byMaxTextLenDecimals = 0;
    m_dLowerLimit = m_dUpperLimit = 0;
    break;
  default: 
    break;
  }
  if (byMaxTextLenDecimals > 7) byMaxTextLenDecimals = 7;
  if (byMaxTextLenDecimals > 0) byMaxTextLenDecimals--; // 1 char for .
  if (i_byLenDecimals > byMaxTextLenDecimals) i_byLenDecimals = byMaxTextLenDecimals;
  m_byTextLenDecimals = i_byLenDecimals;
  m_byTextLenTotal = m_byTextLenNumber + m_byTextLenDecimals;
  if (m_byTextLenDecimals > 0) m_byTextLenTotal++;
  
  Serial << F("len total=") << m_byTextLenTotal << " decimals=" << m_byTextLenDecimals << endl;
  
  m_poDisplay->clear();
  m_poDisplay->setCursor(0, 0);
  if (i_poText)
    *m_poDisplay << i_poText;

  LoadValue ();
  ShowValue ();
}

//----------------------------------------
// Update
//----------------------------------------
void CEditScreen::Update (EnumAction i_enAction)
{
  if (!m_poDisplay) return;
  if (!m_bIsDataExist) return;
  
  Serial << F("CEditScreen::Update(action=") << i_enAction << F(")") << endl;
  switch (i_enAction)
  {
  case EnumAction::EditStart:
    m_iCursorPos = 0;
    break;
  case EnumAction::EditFinish:
    m_iCursorPos = EDITDISABLED;
    break;
  case EnumAction::Clear:
    m_bIsDataExist = false;
    m_iCursorPos = EDITDISABLED;
    break;
  case EnumAction::Select:
    if (m_iCursorPos == m_iCursorPosAbort)
    {
      LoadValue ();
      ShowValue ();
      m_iCursorPos = EDITDISABLED;
    }
    else if (m_iCursorPos == m_iCursorPosSave)
    {
      SaveValue ();
      ShowValue ();
      m_iCursorPos = EDITDISABLED;
    }
    else if (m_iCursorPos < EDITDISABLED)
      m_bSelected = !m_bSelected;
    break;
  case EnumAction::Increment:
    if (m_iCursorPos >= EDITDISABLED) break;
    if (m_bSelected)
      ChangeValue (HIGH);
    else if (m_bInverseNavigation)
      m_iCursorPos--;
    else
      m_iCursorPos++;
    break;
  case EnumAction::Decrement:
    if (m_iCursorPos >= EDITDISABLED) break;
    if (m_bSelected)
      ChangeValue (LOW);
    else if (m_bInverseNavigation)
      m_iCursorPos++;
    else
      m_iCursorPos--;
    break;
  default:
    break;
  }
  
  m_poDisplay->setCursor(m_bySizeX-2, m_byRowMenu);
  if (m_iCursorPos < EDITDISABLED)
  {
    *m_poDisplay << F("AS");
    m_poDisplay->cursor();
    if (m_iCursorPos <  0)                   m_iCursorPos = 0x100 + m_bySizeX-1;
    if (m_iCursorPos == m_byTextLenTotal)    m_iCursorPos = 0x100 + m_bySizeX-2;
    if (m_iCursorPos == 0x100 + m_bySizeX-3) m_iCursorPos = m_byTextLenTotal-1;
    if (m_iCursorPos >  0x100 + m_bySizeX-1) m_iCursorPos = 0;
  }
  else
  {
    *m_poDisplay << F("  ");
    m_poDisplay->noCursor();
    m_bSelected = false;
  }
  
  if (m_bSelected)
    m_poDisplay->blink();
  else
    m_poDisplay->noBlink();

  byte byCursorPosX =  m_iCursorPos & 0xFF;
  byte byCursorPosY = (m_iCursorPos < 0x100 ? 1 : m_byRowMenu);
  Serial << F("cursor pos ") << m_iCursorPos << F(", x=") << byCursorPosX << F(" y=") << byCursorPosY << endl;
  m_poDisplay->setCursor (byCursorPosX, byCursorPosY);
}

//----------------------------------------
// ShowValue
//----------------------------------------
void CEditScreen::ShowValue ()
{
  Serial << F("datatype=") << m_enDataType << endl;

  m_poDisplay->setCursor(0, 1);
  switch (m_enDataType)
  {
  case EnumDataType::BOOL:
    *m_poDisplay << (*(bool*)m_pbyValue ? F("ON ") : F("OFF"));
    break;
  case EnumDataType::BYTE:
    *m_poDisplay << String(*m_pbyValue).padLeftC(m_byTextLenTotal).keepRightC(m_byTextLenTotal);
    break;
  case EnumDataType::INT:
    *m_poDisplay << String(*(int*)m_pbyValue).padLeftC(m_byTextLenTotal).keepRightC(m_byTextLenTotal);
    break;
  case EnumDataType::UINT:
    *m_poDisplay << String(*(unsigned int*)m_pbyValue).padLeftC(m_byTextLenTotal).keepRightC(m_byTextLenTotal);
    break;
  case EnumDataType::LONG:
    *m_poDisplay << String(*(long*)m_pbyValue).padLeftC(m_byTextLenTotal).keepRightC(m_byTextLenTotal);
    break;
  case EnumDataType::ULONG:
    *m_poDisplay << String(*(unsigned long*)m_pbyValue).padLeftC(m_byTextLenTotal).keepRightC(m_byTextLenTotal);
    break;
  case EnumDataType::FLOAT:
    *m_poDisplay << String(*(float*)m_pbyValue, m_byTextLenDecimals).padLeftC(m_byTextLenTotal).keepRightC(m_byTextLenTotal);
    break;
  case EnumDataType::DOUBLE:
    *m_poDisplay << String(*(double*)m_pbyValue, m_byTextLenDecimals).padLeftC(m_byTextLenTotal).keepRightC(m_byTextLenTotal);
    break;
  case EnumDataType::BYTES:
    for (int ixCnt = 0; ixCnt < m_byDataSize; ixCnt++)
      *m_poDisplay << String(*(m_pbyValue + ixCnt), HEX).padLeftC(2, '0');
    break;
  default:
    break;
  }
}

//----------------------------------------
// LoadValue
//----------------------------------------
void CEditScreen::LoadValue ()
{
  if (m_byDataSize > m_byStorageSize)
  {
    byte byStorageSize = m_byDataSize;
    m_byStorageSize = 1;
    while (byStorageSize >>= 1)
      m_byStorageSize = (m_byStorageSize << 1) | 1;
  }
  Serial << F("DataSize=") << m_byDataSize << F(" StorageSize=") << m_byStorageSize << endl;
  m_pbyValue = (byte*)realloc (m_pbyValue, m_byStorageSize);
  if (!m_pbyValue) return;
  memcpy (m_pbyValue, m_pbyValueOrig, m_byDataSize);
}

//----------------------------------------
// SaveValue
//----------------------------------------
void CEditScreen::SaveValue ()
{
  memcpy (m_pbyValueOrig, m_pbyValue, m_byDataSize);
  m_bIsDataChanged = true;
}

//----------------------------------------
// ChangeValue
//----------------------------------------
void CEditScreen::ChangeValue (bool i_bDirection)
{
  Serial << F("cursorpos=") << m_iCursorPos << F(",dir=") << i_bDirection;
  if (m_iCursorPos >= m_byTextLenTotal) return; 
  char cPos = m_byTextLenNumber - m_iCursorPos;
  double dChange = (cPos != 0 ? 1 : 0);
  while (cPos > 1) { cPos--; dChange *= 10;}
  while (cPos < 0) { cPos++; dChange /= 10;}
  if (!i_bDirection) dChange = -dChange;
  Serial << F(" change=") << String(dChange, 7) << endl;
  
  switch (m_enDataType)
  {
  case EnumDataType::BOOL:
    *(bool*)m_pbyValue = !(*(bool*)m_pbyValue);
    break;
  case EnumDataType::BYTE: {
      byte* pbyValue = m_pbyValue;
      if (dChange > 0) {
        if (*pbyValue + dChange <= m_dUpperLimit)
          *pbyValue += (byte)dChange;
        else
          *pbyValue = m_dUpperLimit;
      } else {
        if (*pbyValue + dChange >= m_dLowerLimit)
          *pbyValue += (byte)dChange;
        else
          *pbyValue = m_dLowerLimit;
      }
    } break;
  case EnumDataType::INT: {
      int* piValue = (int*)m_pbyValue;
      if (dChange > 0) {
        if (*piValue + dChange <= m_dUpperLimit)
          *piValue += (int)dChange;
        else
          *piValue = m_dUpperLimit;
      } else {
        if (*piValue + dChange >= m_dLowerLimit)
          *piValue += (int)dChange;
        else
          *piValue = m_dLowerLimit;
      }
    } break;
  case EnumDataType::UINT: {
      unsigned int* puiValue = (unsigned int*)m_pbyValue;
      if (dChange > 0) {
        if (*puiValue + dChange <= m_dUpperLimit)
          *puiValue += (int)dChange;
        else
          *puiValue = m_dUpperLimit;
      } else {
        if (*puiValue + dChange >= m_dLowerLimit)
          *puiValue += (int)dChange;
        else
          *puiValue = m_dLowerLimit;
      }
    } break;
  case EnumDataType::LONG: {
      long* plValue = (long*)m_pbyValue;
      if (dChange > 0) {
        if (*plValue + dChange <= m_dUpperLimit)
          *plValue += (long)dChange;
        else
          *plValue = m_dUpperLimit;
      } else {
        if (*plValue + dChange >= m_dLowerLimit)
          *plValue += (long)dChange;
        else
          *plValue = m_dLowerLimit;
      }
    } break;
  case EnumDataType::ULONG: {
      unsigned long* pulValue = (unsigned long*)m_pbyValue;
      if (dChange > 0) {
        if (*pulValue + dChange <= m_dUpperLimit)
          *pulValue += (long)dChange;
        else
          *pulValue = m_dUpperLimit;
      } else {
        if (*pulValue + dChange >= m_dLowerLimit)
          *pulValue += (long)dChange;
        else
          *pulValue = m_dLowerLimit;
      }
    } break;
  case EnumDataType::FLOAT: {
      float* pfValue = (float*)m_pbyValue;
      *pfValue += (float)dChange;
      if (*pfValue < m_dLowerLimit) *pfValue = (float)m_dLowerLimit;
      if (*pfValue > m_dUpperLimit) *pfValue = (float)m_dUpperLimit;
    } break;
  case EnumDataType::DOUBLE: {
      double* pdValue = (double*)m_pbyValue;
      *pdValue += dChange;
      if (*pdValue < m_dLowerLimit) *pdValue = m_dLowerLimit;
      if (*pdValue > m_dUpperLimit) *pdValue = m_dUpperLimit;
    } break;
  case EnumDataType::BYTES: {
      byte byByte  = m_iCursorPos / 2;
      byte byShift = m_iCursorPos % 2 ? 0 : 4;
      byte byValueNew = *(m_pbyValue + byByte) >> byShift & 0xF;
      byValueNew += i_bDirection ? 1 : -1;
      *(m_pbyValue + byByte) &= 0xF << (4-byShift);
      *(m_pbyValue + byByte) |= (byValueNew & 0xF) << byShift;
    } break;
  default:
    break;
  }

  ShowValue();
}
