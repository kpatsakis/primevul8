inet_ntop2(uint32_t ip)
{
	static char buf[16];
	unsigned char *bytep;

	bytep = (unsigned char *) &(ip);
	sprintf(buf, "%d.%d.%d.%d", bytep[0], bytep[1], bytep[2], bytep[3]);
	return buf;
}