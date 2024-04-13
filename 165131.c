status af_read_int8 (int8_t *value, AFvirtualfile *vf)
{
	if (!read(vf, value))
		return AF_FAIL;
	return AF_SUCCEED;
}