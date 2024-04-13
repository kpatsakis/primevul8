inline static char xml_decode_iso_8859_1(unsigned short c)
{
	return (char)(c > 0xff ? '?' : c);
}