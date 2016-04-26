#include <Streaming.h>
#include "CDoubleOutput.h"

CDoubleOutput m_oMischer (10, 11, false, 1000, 2000);

void setup()
{
  Serial.begin (9600);
  pinMode (5, INPUT);
  pinMode (6, INPUT);
}

bool m_bKey = false;
unsigned long lLoop = 0;

void loop()
{
  bool bKey1 = digitalRead(5);
  bool bKey2 = digitalRead(6);

  if (lLoop == 200) bKey = true;

  if (bKey1 || bKey2)
  {
    if (!m_bKey)
    {
      Serial << F("key1=") << bKey1 << F(" key2=") << bKey2 << endl;
      if (!m_oMischer.IsActive())
        m_oMischer.Write (bKey1, bKey2, true);
    }
    m_bKey = true;
  }
  else
    m_bKey = false;

  m_oMischer.Write ();
  delay (50);
}


