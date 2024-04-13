ncp__vol2io(unsigned char *iname, unsigned int *ilen,
		const unsigned char *vname, unsigned int vlen, int cc)
{
	int i;

	if (*ilen <= vlen)
		return -ENAMETOOLONG;

	if (cc)
		for (i = 0; i < vlen; i++) {
			*iname = tolower(*vname);
			iname++;
			vname++;
		}
	else {
		memmove(iname, vname, vlen);
		iname += vlen;
	}

	*ilen = vlen;
	*iname = 0;
	return 0;
}