status af_write_pstring (const char *s, AFvirtualfile *vf)
{
	size_t length = strlen(s);
	if (length > 255)
		return AF_FAIL;
	uint8_t sizeByte = (uint8_t) length;
	if (vf->write(&sizeByte, 1) != 1)
		return AF_FAIL;
	if (vf->write(s, length) != (ssize_t) length)
		return AF_FAIL;
	/*
		Add a pad byte if the length of the Pascal-style string
		(including the size byte) is odd.
	*/
	if ((length % 2) == 0)
	{
		uint8_t zero = 0;
		if (vf->write(&zero, 1) != 1)
			return AF_FAIL;
	}
	return AF_SUCCEED;
}