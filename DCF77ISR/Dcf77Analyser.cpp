#include <arduino.h>


#include "Dcf77Analyser.h"

#define DEBUGTIMING 1

static const uint8_t bcdMulti[8] = {1, 2, 4, 8, 10, 20, 40, 80};

Dcf77Analyser::Dcf77Analyser()
	: m_lastMin(0)
	, m_hour(0)
	, m_min(0)
	, m_day(0)
	, m_month(0)
	, m_year(0)
	, m_weekday(0)
	, m_parity_hour(false)
	, m_parity_min(false)
	, m_parity_date(false)
{
	ResetVector();
}

void Dcf77Analyser::ResetVector()
{
	for (int i; i < sizeof(m_fMatrix)-1; ++i)
	{
		m_fMatrix[i] = '-';
	}
	m_fMatrix[61] = 0;
}

uint8_t Dcf77Analyser::bcdbitsToInt(char *data, uint8_t size) const
{
	uint8_t val = 0;
	uint8_t pos = 0;
	do
	{
		if (*data == '1') {
			val += bcdMulti[pos];
		}
		else if (*data != '0') {
			return -1;
		}
		++data;
		++pos;
	} while (--size);

	return val;
}

bool Dcf77Analyser::checkParity(char *data, uint8_t size, char par) const
{
	uint8_t val = 0;
	if (par == '1') {
		par = 1;
	}
	else if (par == '0') {
		par = 0;
	}
	else
		return false;
	while (size--) {
		switch (*data) {
		case '1': val ^= 1; break;
		case '0': break; // normal assigned 0
		default: return false; // not assigned or doubtly assigned
		}
		data++;
	}
	return val == par;
}

bool Dcf77Analyser::isMinute() const
{
	return m_isMinute;
}

void Dcf77Analyser::minuteRead()
{
	m_isMinute = false;
}


void Dcf77Analyser::feed(uint32_t msecs, uint16_t dt)
{
	uint32_t slot = static_cast<float>(msecs - m_lastMin) * 60000.0 / (60000.0 - m_driftCorrectionPerMinute);
	if (dt < 10)
		return;
	m_currentSec = (slot / 1000) % 60;
	m_cntIsr++;
	if (dt > 1700 && dt < 1950)
	{
		if ((msecs - m_lastMin >= 59000) && (msecs - m_lastMin < 61000))
		{
			int32_t drift = 60000 - (msecs - m_lastMin);
			m_drift.AddValue(static_cast<int16_t>(drift));
			m_driftCorrectionPerMinute = m_drift.getAvg();
#if DEBUGTIMING
			{
				char outb[64];
				sprintf(outb, "drift %ld", drift);
				Serial.println(outb);
				sprintf(outb, "now: %d", m_driftCorrectionPerMinute);
				Serial.println(outb);
			}
#endif
		}
		m_lastMin = msecs;
		m_isMinute = true;
#if DEBUGTIMING
		//if (digitalRead(DEBUG_Pin) == 0)
		{
			char outb[40];
			sprintf(outb, "minpause: %ld %d %d", slot, m_currentSec, slot % 1000);
			Serial.println(outb);
		}
		{
			char outb[40];
			sprintf(outb, "bit 0 t-avg: %d",m_value0.getAvg());
			Serial.println(outb);
			sprintf(outb, "bit 1 t-avg: %d",m_value1.getAvg());
			Serial.println(outb);

		}
#endif
	}
	if (m_lastMin != 0)
	{
		uint16_t index = slot/1000;
		if (index <=180)
		{
			uint16_t modslot = slot % 1000;
			if (modslot >= 130-20 && modslot <= 130+20)
			{
				m_value0.AddValue(modslot);
				m_snap = modslot;
				m_fMatrix[index%60] = '0';
#if DEBUGTIMING
				{		
					char outb[40];
					sprintf(outb, "bit0: %d sec:%d", modslot-130, m_currentSec);
					Serial.println(outb);
				}
#endif	
			}
			if (modslot >= 226-20 && modslot <= 226+20)
			{
				m_value1.AddValue(modslot);
				m_snap = modslot;
				if (m_fMatrix[index%60] == '0')
					m_fMatrix[index%60] = 'X';
				else
					m_fMatrix[index%60] = '1';
#if DEBUGTIMING
				{		
					char outb[40];
					sprintf(outb, "bit1: %d sec:%d", modslot-226, m_currentSec);
					Serial.println(outb);
				}
#endif	
			}
		}
	}
}

void Dcf77Analyser::elaborate()
{
	m_min = bcdbitsToInt(m_fMatrix + 21, 7);
	m_hour = bcdbitsToInt(m_fMatrix + 29, 6);
	m_day = bcdbitsToInt(m_fMatrix + 36, 6);
	m_month = bcdbitsToInt(m_fMatrix + 45, 5);
	m_year = bcdbitsToInt(m_fMatrix + 50, 8) + 2000;
	m_weekday = bcdbitsToInt(m_fMatrix + 42, 3);
	m_parity_hour = checkParity(m_fMatrix + 29, 6, m_fMatrix[35]);
	m_parity_min = checkParity(m_fMatrix + 21, 7, m_fMatrix[28]);
	m_parity_date = checkParity(m_fMatrix + 36, 58 - 36, m_fMatrix[58]);

	ResetVector();

#if DEBUGTIMING
	// char outb[64];
	//sprintf(outb, "%2d:%02d %02d %02d %04d (%d)\t" , m_hour, m_min, m_day, m_month, year, weekday) ;
	//Serial.println(outb);
	//              012345678901234567890123456789012345678901234567890123456789
	Serial.println("0              A ST 1mmmmmmmPhhhhhhPddddddwwwMMMMMyyyyyyyyP");
	Serial.println(m_fMatrix);
#endif

}
