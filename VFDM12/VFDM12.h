
#ifndef _VFDM12_h
#define _VFDM12_h



class VFDM12
{
private:

	char m_curadr;
	int m_vfdaddr;
public:
	VFDM12();
	void init();
	void init(int adr);
	void sendSegments(char adress, char c, char d, char e); // e is always empty
	void sendLed(char c);
	void clear();
	void sendChar(char c);
	void writeString(char *str);
};

#endif //_VFDM12_h
