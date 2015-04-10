
template <typename _T_, uint16_t _SZ_, uint16_t _OFFSET_, uint16_t _TURNAROUND_>
class DataPlausibility
{
public:
	DataPlausibility()
		: m_currentWritePos(0)
	{

	}

	void AddValue(const _T_ & value)
	{
		if (m_currentWritePos>=_SZ_)
		{
			--m_currentWritePos;
			for (int i=0; i < _SZ_-1; ++i)
			{
				m_data[i] = m_data[i+1];
			}
		}
		m_data[m_currentWritePos++] = value;
	}

	bool isPlausible(int maxDiff=0) const
	{
		int errorCount = 0;
		for (int i=0; i < _SZ_-1; ++i)
		{
			if (m_data[i] != (m_data[i+1] + _TURNAROUND_ - _OFFSET_) % _TURNAROUND_) {
				++errorCount;
			}
		}
		return errorCount <= maxDiff;
	}
private:
	int m_currentWritePos;
	_T_ m_data[_SZ_];

};
