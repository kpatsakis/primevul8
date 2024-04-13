status af_read_uint8 (uint8_t *value, AFvirtualfile *vf)
{
	if (!read(vf, value))
		return AF_FAIL;
	return AF_SUCCEED;
}