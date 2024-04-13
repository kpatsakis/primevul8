inline float _af_byteswap_float32 (float x)
{
	union
	{
		uint32_t i;
		float f;
	} u;
	u.f = x;
	u.i = _af_byteswap_int32(u.i);
	return u.f;
}