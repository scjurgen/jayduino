#include "../DataPlausibilityTemplate.h"


template <typename _T_, size_t _SZ_, size_t _OFFSET_, size_t _TURNAROUND_>
class DataPlausibility

DataPlausibility<int, 4,1,60> minutes;

void testWithIncrement()
{
	minutes.AddValue(10);
	minutes.AddValue(11);
	minutes.AddValue(12);
	minutes.AddValue(13);
	
}



int main(int, char *[])
{
	return 0;
}
