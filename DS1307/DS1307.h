#pragma once


// library interface description
class DS1307
{
  public:
  	typedef enum
  	{
  		HZ1,
  		HZ4096,
  		HZ8192,
  		HZ32768
  	} SquareWaveFrequency;
  	
    DS1307();
    void start(void);
    void stop(void);
    void setControlRegister(unsigned char);
    void StartSquareWaveInterrupt(SquareWaveFrequency freq, bool inverseFlank = false);
    void StopSquareWaveInterrupt();
    // the DS1307 provides 56 bytes of memory
    void readSRAM(unsigned char *mem) const; // we are talking to hardware, lets pretend the class expands to it, therefore const and non const
    void writeSRAM(const unsigned char *mem);
  	
  	//!call once refresh before reading data
    void refresh();
  	//!return converted seconds 
  	inline bool stopped() const {return dateTimeBCD.second*0x80?true:false;}
  	//!return second (since refresh)
  	inline unsigned char second() const    {return 10*((dateTimeBCD.second & 0x70)>>4) + (dateTimeBCD.second  & 0x0f);}
  	//!return minute
  	inline unsigned char minute() const    {return 10*((dateTimeBCD.minute & 0x70)>>4) + (dateTimeBCD.minute  & 0x0f);}
  	//!return hour (be aware that when set to ampm mode this is limited to 0..12
  	inline unsigned char hour() const      {
  		if (dateTimeBCD.hour & 0x40)
	  		return dateTimeBCD.hour & 0x0f; // ampm mode
  		else
	  		return 10*((dateTimeBCD.hour    & 0x30)>>4) + (dateTimeBCD.hour    & 0x0f);
	}
  	//!return dayofweek from 1 to 7
  	inline unsigned char dayOfWeek() const {return                                         dateTimeBCD.dayOfWeek & 0x07;}
  	//!return day
  	inline unsigned char day() const       {return 10*((dateTimeBCD.day     & 0x30)>>4) + (dateTimeBCD.day     & 0x0f);}
  	//!return month
  	inline unsigned char month() const     {return 10*((dateTimeBCD.month   & 0x30)>>4) + (dateTimeBCD.month   & 0x0f);}
  	//!return year
  	inline unsigned char year() const      {return 10*((dateTimeBCD.year    & 0xF0)>>4) + (dateTimeBCD.year    & 0x0f);}

  	//!return ampm as character 'A' : 'P', and '?' if not ampm is selected

	inline unsigned char AmPmSuffix() const
	{
		if (dateTimeBCD.hour & 0x40) 
			
			return dateTimeBCD.hour & 0x20?'P':'A';
		return '?';
	};

  protected:
  	struct
  	{
  		unsigned char second;
  		unsigned char minute;
  		unsigned char hour;
  		unsigned char dayOfWeek;
  		unsigned char day;
  		unsigned char month;
  		unsigned char year;
  	} dateTimeBCD;
  	
    void read(void);
    void write(void);
};

