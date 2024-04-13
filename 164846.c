status af_read_pstring (char s[256], AFvirtualfile *vf)
{
	uint8_t length;
	/* Read the Pascal-style string containing the name. */
	if (vf->read(&length, 1) != 1)
		return AF_FAIL;
	if (vf->read(s, length) != (ssize_t) length)
		return AF_FAIL;
	s[length] = '\0';
	return AF_SUCCEED;
}