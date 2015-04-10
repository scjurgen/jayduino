#include <avr/pgmspace.h>
#include <wire.h>
#include "vfdascii.h"
#include "VFDM12.H"

VFDM12::VFDM12() 
{
}

void VFDM12::init(int adr)
{
	m_vfdaddr=adr;
	Wire.beginTransmission(m_vfdaddr);
  //Wire.write(0x00);
	Wire.write(0x9f);
	int result = Wire.endTransmission();
	clear();
}

void VFDM12::init()
{
	init(0xa0 >> 1);
}


void VFDM12::sendSegments(char adress, char c, char d, char e)
{
    Wire.beginTransmission(m_vfdaddr);
    Wire.write(0xc0+adress);
    Wire.write(c);
    Wire.write(d);
    Wire.write(e);
    int result = Wire.endTransmission();
    
}


void VFDM12::sendLed(char c)
{
    Wire.beginTransmission(m_vfdaddr);
    Wire.write(0x41);
    Wire.write(c);
    int result = Wire.endTransmission();
}

void VFDM12::clear()
{
    for (int i=0; i < 36; i+=3)
        sendSegments(i, 0,0,0);
}

void VFDM12::sendChar(char c)
{
    if (m_curadr <0)
    {
        m_curadr = 33;
    }
    if (c >=0)
    {
        int o=c*2;
//        vfd_send_segments(vfd_curadr, vfdchars[o], vfdchars[o+1], 0);
        sendSegments(m_curadr, pgm_read_byte(&m_vfdchars[o]), pgm_read_byte(&m_vfdchars[o+1]), 0);
        m_curadr -= 0x03;
    }
}

void VFDM12::writeString(char *str)
{
    m_curadr=11*3;
    while (*str)
    {
        int o=*str++ * 2;
        sendSegments(m_curadr, pgm_read_byte(&m_vfdchars[o]), pgm_read_byte(&m_vfdchars[o+1]), 0);
        if (m_curadr==0)
            return;
        m_curadr -= 0x03;
    }
}

