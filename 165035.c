status af_write_int8 (const int8_t *value, AFvirtualfile *vf)
{
	if (!write(vf, value))
		return AF_FAIL;
	return AF_SUCCEED;
}