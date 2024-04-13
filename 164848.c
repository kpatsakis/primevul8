	void getDefaultPCMMapping(int &bits, double &slope, double &intercept,
		double &minClip, double &maxClip)
	{
		bits = (m_outFormat + 1) * CHAR_BIT;
		slope = (1LL << (bits - 1));
		intercept = 0;
		minClip = -(1 << (bits - 1));
		maxClip = (1LL << (bits - 1)) - 1;
	}