#include <OneWire.h>
#include <Streaming.h>
#include "CTempDS18B20.h"

OneWire m_oOneWire(2);
void setup()
{
  Serial.begin (9600);
}

void loop()
{
  byte byDeviceCount = 0;
  byte aabyDevAddr[5][8];
  Serial << F("Searching 1-wire devices...") << endl;

  CTempDS18B20::EnumResult enResultDS18B20 = CTempDS18B20::DeviceSearch (&m_oOneWire, aabyDevAddr, byDeviceCount);
  Serial << byDeviceCount << F(" devices found.") << endl;

  for (byte byDevice = 0; byDevice < byDeviceCount; byDevice++)
  {
    byte* abyDevAddr = aabyDevAddr[byDevice];

    CTempDS18B20 oTemp (&m_oOneWire, abyDevAddr, enResultDS18B20);
    Serial << F("Device ") << byDevice << F(" created: ") << enResultDS18B20 << endl;

    oTemp.Init ();

    float fTemp = 0.0;
    enResultDS18B20 = oTemp.ReadTemp (true, fTemp);
    Serial << F("result=") << enResultDS18B20 << F(" temp=") << fTemp << endl;

    delay (1000);
  }

  delay (5000);
}

//Searching 1-wire devices...
//Device 0, address 28 ff e1 da 81 15 01 c1
//CRC=0xc1
//Device 1, address 28 ff c5 95 61 15 02 79
//CRC=0x79
//Device 2, address 28 ff a7 9a 61 15 02 71
//CRC=0x71
//3 devices found.
//
//Device 0
//t reset=1
//t select=5
//t write(0x44)=0
//100: 0
//200: 0
//300: 0
//400: 0
//500: 0
//600: 0
//700: 1
//t reset=1
//t select=5
//t write(0xBE)=1
//t read 9 bytes=5
//0x 49 01 4B 46 7F FF 0C 10 D5
//Temp=20.56
//CRC=0xD5
//
//Device 1
//t reset=1
//t select=6
//t write(0x44)=0
//100: 0
//200: 0
//300: 0
//400: 0
//500: 0
//600: 0
//700: 1
//t reset=1
//t select=6
//t write(0xBE)=0
//t read 9 bytes=5
//0x 47 01 4B 01 7F FF 0C 10 D4
//Temp=20.44
//CRC=0xD4
//
//Device 2
//t reset=1
//t select=6
//t write(0x44)=0
//100: 0
//200: 0
//300: 0
//400: 0
//500: 0
//600: 0
//700: 1
//t reset=1
//t select=5
//t write(0xBE)=0
//t read 9 bytes=5
//0x 48 01 4B 01 7F FF 0C 10 2E
//Temp=20.50
//CRC=0x2E
//

