	void extract3(T in, uint8_t &c0, uint8_t &c1, uint8_t &c2)
	{
#ifdef WORDS_BIGENDIAN
			c0 = (in >> 16) & 0xff;
			c1 = (in >> 8) & 0xff;
			c2 = in & 0xff;
#else
			c2 = (in >> 16) & 0xff;
			c1 = (in >> 8) & 0xff;
			c0 = in & 0xff;
#endif
	}