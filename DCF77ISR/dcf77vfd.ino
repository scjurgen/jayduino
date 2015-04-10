
#include <arduino.h>
#include <Wire.h>
#include <stdint.h>
#include "Dcf77Analyser.h"

#include "VFDM12.h"
#include "DS1307.h"

VFDM12 vfd;

#define DEBUG_Pin 8
#define DCF77_Pin 2
#define DCF77_INTNR 0


Dcf77Analyser dcf;
volatile static uint32_t currentInterruptTime;
volatile static uint32_t lastInterrupt;

void dcfInterrupt()
{
	uint32_t currentTime = millis();
	dcf.feed(currentTime, currentTime-currentInterruptTime);
	if (digitalRead(DEBUG_Pin) == 0)
	{
		char outb[64];
		sprintf(outb, "%ld %d %d %d", currentTime, dcf.m_currentSec, dcf.m_snap, currentTime-currentInterruptTime);
		Serial.println(outb);
		dcf.m_snap = 0;
	}
	currentInterruptTime = currentTime;
}

DS1307 realTimeClock;

void ShowRTC(void)
{
	char outb[20];
	sprintf(outb, "  %2d:%02d:%02d  ",
		realTimeClock.hour(), 
		realTimeClock.minute(),
		realTimeClock.second()); 
	vfd.writeString(outb);
	//Serial.println(outb);
}

const char *days[] = {"??", "MO","TU","WE", "TH", "FR", "SA", "SU"};

void ShowRTCDate(void)
{
	char outb[20];
	sprintf(outb, "%2d-%2d-%02d %s",
		realTimeClock.day(), 
		realTimeClock.month(), 
		realTimeClock.year()-2000,
		days[realTimeClock.dayOfWeek()]); 
	vfd.writeString(outb);
}
int hour = 0, min = 0, seconds = 0, day = 0, month = 0;

void setup()
{
	Wire.begin();
	vfd.init();
	Serial.begin(115200);
	vfd.writeString("STARTED");
	Serial.println("Start");
	pinMode(DCF77_Pin, INPUT);
	pinMode(DEBUG_Pin, INPUT);
	digitalWrite(DEBUG_Pin, HIGH); 
	digitalWrite(DCF77_Pin, HIGH); 
	attachInterrupt(DCF77_INTNR, dcfInterrupt, CHANGE);
	realTimeClock.refresh();
	hour = realTimeClock.hour();
	min = realTimeClock.minute();
	seconds = realTimeClock.second();
	day = realTimeClock.day();
	month = realTimeClock.month();
	ShowRTCDate();
	delay(1000);
}

uint32_t lastSecondShown = 0;

DataPlausibility<int, 3,1,60> minutesPlausibility;
DataPlausibility<int, 3,0,24> hoursPlausibility;

uint32_t resetStuffNow = 0;

void loop(void)
{
	if (resetStuffNow)
	{
		if (millis()>resetStuffNow)
		{
			resetStuffNow = 0;
			realTimeClock.refresh();
			int curSec = realTimeClock.second();
			if (abs(curSec-30)>2) // correct if we have a problem of more than 2 seconds
			{			
				realTimeClock.setSecond(30);
				Serial.print("Current seconds: ");
				Serial.print(curSec);
				Serial.println(" seconds assigned with 30 sec delay");
			}
			if (dcf.m_parity_hour)
			{
				hoursPlausibility.AddValue(dcf.m_hour);
				if (hoursPlausibility.isPlausible(0))
				{
						//Serial.print(dcf.m_hour);
						if (dcf.m_hour == hour) {
							Serial.println(" hours verified");						
						}
						else {	
							Serial.println(" hours assigned");
						}
						realTimeClock.setHour24(dcf.m_hour);
						hour = dcf.m_hour;
				}
			}
			else
			{
				hoursPlausibility.AddValue(realTimeClock.hour());
			}
			if (dcf.m_parity_min)
			{
				char outb[32];
				sprintf(outb,"Min par: %d", dcf.m_min);
				Serial.println(outb);
				minutesPlausibility.AddValue(dcf.m_min);
				if (minutesPlausibility.isPlausible(0))
				{
					if (dcf.m_min == (min+1)%60) {
						Serial.println("minutes verified");						
					}
					else {	
						Serial.println("minutes assigned");
					}
					if (dcf.m_min != 59)
						min = dcf.m_min;
					else
						min = 0;
					realTimeClock.setMinute(min);
				}				
			}
			else {
				++min;
				minutesPlausibility.AddValue(min);
			}
			if (dcf.m_parity_date)
			{
				realTimeClock.setDay(dcf.m_day);
				realTimeClock.setMonth(dcf.m_month);
				realTimeClock.setYear(dcf.m_year-2000);
				realTimeClock.setDayOfWeek(dcf.m_weekday+1);
				day = dcf.m_day;
				month = dcf.m_month;
			}
		}
	}
	if (dcf.isMinute())
	{
		resetStuffNow = millis()+30000;
		dcf.elaborate();
		dcf.minuteRead();
		seconds = 0;
	}
	static uint32_t nextStats=0;
	if (millis() > nextStats)
	{
		realTimeClock.refresh();
		if (abs(realTimeClock.second()-15)<3) { 
			ShowRTCDate();
		}
		else {
			ShowRTC();
		}
//		char outb[64];
//		sprintf(outb,"isr=%d sec=%d",dcf.m_cntIsr,dcf.m_currentSec);
		if (dcf.m_cntIsr == 2)
			vfd.sendLed(2);
		else
		if (dcf.m_cntIsr <2)
			vfd.sendLed(1);
		else
		if (dcf.m_cntIsr >3)
			vfd.sendLed(4);
		dcf.m_cntIsr = 0;
		nextStats = millis() + 1000;
//		Serial.println(outb);
	}
}

