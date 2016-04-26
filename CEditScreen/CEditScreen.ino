#include <Streaming.h>
#include <LiquidCrystal_I2C.h>
#include "CEditScreen.h"

const int I2C_Addr_Display    = 0x27;

LiquidCrystal_I2C   m_oDisplay (I2C_Addr_Display, 16, 2);
CEditScreen         m_oEditScreen (&m_oDisplay, 16, true);


CEditScreen::EnumAction m_enInput = CEditScreen::EnumAction::None;
long lValue = 123456789;
float fValue = 1256.78;
float m_fTWasserSoll = 45;
byte aby[6] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC};
int  m_iScreen = 0;
byte bValue = 100;
unsigned int uiValue = 234;
int iValue = 100;

void setup() 
{
  Serial.begin (9600);
  pinMode (4, INPUT);
  pinMode (5, INPUT);
  pinMode (6, INPUT);
  m_oDisplay.init();
  m_oDisplay.backlight();
  Serial << F("setup completed.") << endl;
}

void loop()
{
  CEditScreen::EnumAction enAction = CEditScreen::EnumAction::None;
  CEditScreen::EnumAction enInput  = CEditScreen::EnumAction::None;
  if (digitalRead(4))
  {
    if (m_oEditScreen.IsEditActive())
      enInput = CEditScreen::EnumAction::Select;
    else
      enInput = CEditScreen::EnumAction::EditStart;
  }
  if (digitalRead(5)) enInput = CEditScreen::EnumAction::Increment;
  if (digitalRead(6)) enInput = CEditScreen::EnumAction::Decrement;
  if (enInput != m_enInput)
  {
    enAction = enInput;
    m_enInput = enInput; 
    Serial << "key=" << enAction << endl;
  }
  
  if (m_oEditScreen.IsEditActive())
  {
    if (enAction != CEditScreen::EnumAction::None)
    {
      m_oEditScreen.Update (enAction);
      if (m_oEditScreen.IsDataChanged())
      {
        Serial << "Data changed." << endl;
      }
    }
  }
  else
  {
    int iScr = -1;
    if (enAction == CEditScreen::EnumAction::Increment)
    {
      m_iScreen++;
      Serial << m_iScreen << endl;
      iScr = m_iScreen;
    }
    if (enAction == CEditScreen::EnumAction::Decrement)
    {
      m_iScreen--; 
      if (m_iScreen < 0) m_iScreen = 0;
      Serial << m_iScreen << endl;
      iScr = m_iScreen;
    }
    if (enAction == CEditScreen::EnumAction::EditStart)
    {
      Serial << "EditStart" << endl;
      m_oEditScreen.Update (enAction);      
    }
    
    switch (iScr)
    {
    case 0:
    case 8:
      m_oDisplay.clear();
      break;
    case 1:
      m_oEditScreen.Show (F("byte test"), CEditScreen::EnumDataType::BYTES, aby, 0, 0, 6, 0, true);
      break;
    case 2:
      m_oEditScreen.Show (F("long"), CEditScreen::EnumDataType::LONG, (byte*)&lValue, 1000000, 0, 7, 0, true);
      break;
    case 3:
      m_oEditScreen.Show (F("float"), CEditScreen::EnumDataType::FLOAT, (byte*)&fValue, 50000, -70000, 8, 2, false);
      break;
    case 4:
      m_oEditScreen.Show (F("T.Soll Wasser"), CEditScreen::EnumDataType::FLOAT, (byte*)&m_fTWasserSoll, 20, 80, 3, 1, true);
      break;
    case 5:
      m_oEditScreen.Show (F("byte"), CEditScreen::EnumDataType::BYTE, &bValue, 200, 0, 3, 0, false);
      break;
    case 6:
      m_oEditScreen.Show (F("int"), CEditScreen::EnumDataType::UINT, (byte*)&uiValue, 60000, 10, 6, 0, false);
      break;
    case 7:
      m_oEditScreen.Show (F("int"), CEditScreen::EnumDataType::INT, (byte*)&iValue, 30000, 10, 6, 0, false);
      break;
    }
    
  }
  delay (100);
}
