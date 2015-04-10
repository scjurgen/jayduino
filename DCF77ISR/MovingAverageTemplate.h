

template <typename _T_, uint16_t _SZ_>
class Average
{
public:
	Average()
		: m_currentWritePos(-1)
		, m_sum(0)
	{

	}

	void AddValue(const _T_ & value)
	{
		if (m_currentWritePos == -1)
		{
			for (int i = 0; i < _SZ_; ++i)
			{
				m_sum += value;
				m_data[i] = value;
			}
			++m_currentWritePos;
		}
		else
		{
			m_sum -= m_data[m_currentWritePos]; // moving average (subtract old, add new)
			m_sum += value;
			m_data[m_currentWritePos] = value;
			++m_currentWritePos;
			if (m_currentWritePos >= _SZ_) {
				m_currentWritePos = 0;
			}
		}
	}

	_T_ getAvg() const
	{
		return m_sum / _SZ_;
	}

private:

	int m_currentWritePos;
	_T_ m_data[_SZ_];
	_T_ m_sum;

};
