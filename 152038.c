inline static char xml_decode_us_ascii(unsigned short c)
{
	return (char)(c > 0x7f ? '?' : c);
}