/*
  scjurgen@yahoo.com
*/


#include <avr/pgmspace.h>
#include <wire.h>

#include "DS1307.h"

#define DS1307_SEC 0
#define DS1307_MIN 1
#define DS1307_HR 2
#define DS1307_DOW 3
#define DS1307_DATE 4
#define DS1307_MTH 5
#define DS1307_YR 6
#define DS1307_CTRLREG 7


#define DS1307_CTRL_ID 0B1101000  // 0x68 //DS1307

 // Define register bit masks  
#define DS1307_CLOCKHALT 0B10000000 //0x80
#define DS1307_AMPM 0B01000000 
 
#define DS1307_DATASTART 0x08

DS1307::DS1307()
{
  Wire.begin();
  //refresh();
}

void DS1307::refresh()
{
	read();
}

// requests 7 bytes of data: seconds, minutes, hour, day of week, month, year
void DS1307::read(void)
{
  Wire.beginTransmission(DS1307_CTRL_ID);
  Wire.write(0x00);
  Wire.endTransmission();

  Wire.requestFrom(DS1307_CTRL_ID, 7);
  dateTimeBCD.second = Wire.read();
  dateTimeBCD.minute = Wire.read();
  dateTimeBCD.hour = Wire.read();
  dateTimeBCD.dayOfWeek = Wire.read();
  dateTimeBCD.day = Wire.read();
  dateTimeBCD.month = Wire.read();
  dateTimeBCD.year = Wire.read();
  
}



void DS1307::write(void)
{
  Wire.beginTransmission(DS1307_CTRL_ID);
  // buffer pointer register 
  Wire.write(0x00); 
  Wire.write(dateTimeBCD.second);
  Wire.write(dateTimeBCD.minute);
  Wire.write(dateTimeBCD.hour);
  Wire.write(dateTimeBCD.dayOfWeek);
  Wire.write(dateTimeBCD.day);
  Wire.write(dateTimeBCD.month);
  Wire.write(dateTimeBCD.year);
  Wire.endTransmission();
}


/*

void DS1307::set(unsigned char c, unsigned char v)  // Update buffer, then update the chip
{
  switch(c)
  {
  case DS1307_SEC:
    if(v<60 && v>-1)
    {
      //preserve existing clock state (running/stopped)
      int state=rtc_bcd[DS1307_SEC] & DS1307_CLOCKHALT;
      rtc_bcd[DS1307_SEC]=state | (((v / 10)<<4) + (v % 10));
    }
    break;
  case DS1307_MIN:
    if(v<60 && v>-1)
    {
      rtc_bcd[DS1307_MIN]=((v / 10)<<4) + (v % 10);
    }
    break;
  case DS1307_HR:
   // TODO : AM/PM  12HR/24HR currently 24 hour mode
    if(v<24 && v>-1)
    {
      rtc_bcd[DS1307_HR]=((v / 10)<<4) + (v % 10);
    }
    break;
  case DS1307_DOW:
    if(v<8 && v>-1)
    {
      rtc_bcd[DS1307_DOW]=v;
    }
    break;
  case DS1307_DATE:
    if(v<32 && v>-1)
    {
      rtc_bcd[DS1307_DATE]=((v / 10)<<4) + (v % 10);
    }
    break;
  case DS1307_MTH:
    if(v<13 && v>-1)
    {
      rtc_bcd[DS1307_MTH]=((v / 10)<<4) + (v % 10);
    }
    break;
  case DS1307_YR:
    if(v<99 && v>-1)
    {
      rtc_bcd[DS1307_YR]=((v / 10)<<4) + (v % 10);
    }
    break;
  } // end switch
  write();
}
*/
void DS1307::readSRAM(unsigned char *sram_data) const
{
  // set the register to the sram area and read 56 bytes
  Wire.beginTransmission(DS1307_CTRL_ID);
  Wire.write(DS1307_DATASTART);
  Wire.endTransmission();
 
  for(int i=0;i<56;i++)
  {
    Wire.requestFrom(DS1307_CTRL_ID, 56);
    sram_data[i]=Wire.read();
  }
}

void DS1307::writeSRAM(const unsigned char *sram_data)
{
	// set the register to the sram area and write 56 bytes
	Wire.beginTransmission(DS1307_CTRL_ID);
	Wire.write(DS1307_DATASTART);

	for(int i=0;i<56;i++)
	{
		Wire.write(sram_data[i]);
	}
	Wire.endTransmission();
}


// set ctrl register for Clock out (see Macros: DS1307_1HZ etc.)
void DS1307::setControlRegister(unsigned char b)
{
    Wire.beginTransmission(DS1307_CTRL_ID);
    Wire.write(DS1307_CTRLREG);
    Wire.write(b);
    Wire.endTransmission();  
}
