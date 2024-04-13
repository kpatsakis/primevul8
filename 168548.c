ncp__io2vol(unsigned char *vname, unsigned int *vlen,
		const unsigned char *iname, unsigned int ilen, int cc)
{
	int i;

	if (*vlen <= ilen)
		return -ENAMETOOLONG;

	if (cc)
		for (i = 0; i < ilen; i++) {
			*vname = toupper(*iname);
			vname++;
			iname++;
		}
	else {
		memmove(vname, iname, ilen);
		vname += ilen;
	}

	*vlen = ilen;
	*vname = 0;
	return 0;
}