set_str(unsigned char *p, const char *s, size_t l, char f, const char *map)
{
	unsigned char c;

	if (s == NULL)
		s = "";
	while ((c = *s++) != 0 && l > 0) {
		if (c >= 0x80 || map[c] == 0)
		 {
			/* illegal character */
			if (c >= 'a' && c <= 'z') {
				/* convert c from a-z to A-Z */
				c -= 0x20;
			} else
				c = 0x5f;
		}
		*p++ = c;
		l--;
	}
	/* If l isn't zero, fill p buffer by the character
	 * which indicated by f. */
	if (l > 0)
		memset(p , f, l);
}