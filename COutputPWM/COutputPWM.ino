#include <Streaming.h>
#include "COutputPWM.h"

COutputPWM m_oPumpe (12, false, 0, 10);

void setup()
{
  Serial.begin (9600);
}

byte m_byValue = 0;

void loop()
{
  m_oPumpe.Write ();

  if (digitalRead(4))
  {
    m_oPumpe.SetUp (++m_byValue, 10);
    Serial << F("new value: ") << m_byValue << endl;
    delay(1000);
  }
  delay (500);
}


