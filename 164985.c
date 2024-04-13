inline double _af_byteswap_float64 (double x)
{
	union
	{
		uint64_t i;
		double f;
	} u;
	u.f = x;
	u.i = _af_byteswap_int64(u.i);
	return u.f;
}