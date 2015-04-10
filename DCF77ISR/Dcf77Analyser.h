#include <stdint.h>

#include "MovingAverageTemplate.h"
#include "DataPlausibilityTemplate.h"


class Dcf77Analyser
{
public:
	uint16_t m_snap;
	uint32_t m_lastMin;
	
	int16_t m_hour, m_min, m_day, m_month, m_year, m_weekday, m_currentSec;
	bool m_parity_hour;
	bool m_parity_min;
	bool m_parity_date;
	uint16_t m_cntIsr;
	
	Dcf77Analyser();


	bool isMinute() const;
	void minuteRead();
	void feed(uint32_t msecs, uint16_t dt);
	
	void elaborate();	
private:
	void ResetVector();
	uint8_t bcdbitsToInt(char *data, uint8_t size) const;
	bool checkParity(char *data, uint8_t size, char par) const;
	bool m_isMinute;
	int16_t m_driftCorrectionPerMinute;
	char m_fMatrix[62];
	Average<uint16_t, 8> m_drift; // not needed if we use a realtime clock
	Average<uint16_t, 100> m_value0; 
	Average<uint16_t, 100> m_value1; 
};