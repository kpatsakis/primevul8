joliet_allowed_char(unsigned char high, unsigned char low)
{
	int utf16 = (high << 8) | low;

	if (utf16 <= 0x001F)
		return (0);

	switch (utf16) {
	case 0x002A: /* '*' */
	case 0x002F: /* '/' */
	case 0x003A: /* ':' */
	case 0x003B: /* ';' */
	case 0x003F: /* '?' */
	case 0x005C: /* '\' */
		return (0);/* Not allowed. */
	}
	return (1);
}