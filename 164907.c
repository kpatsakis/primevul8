status af_write_uint8 (const uint8_t *value, AFvirtualfile *vf)
{
	if (!write(vf, value))
		return AF_FAIL;
	return AF_SUCCEED;
}