inet_ston(const char *addr, uint32_t * dst)
{
	static char digits[] = "0123456789";
	int saw_digit, octets, ch;
	u_char tmp[INADDRSZ], *tp;

	saw_digit = 0;
	octets = 0;
	*(tp = tmp) = 0;

	while ((ch = *addr++) != '\0' && ch != '/' && ch != '-') {
		const char *pch;
		if ((pch = strchr(digits, ch)) != NULL) {
			u_int new = *tp * 10 + (pch - digits);
			if (new > 255)
				return 0;
			*tp = new;
			if (!saw_digit) {
				if (++octets > 4)
					return 0;
				saw_digit = 1;
			}
		} else if (ch == '.' && saw_digit) {
			if (octets == 4)
				return 0;
			*++tp = 0;
			saw_digit = 0;
		} else
			return 0;
	}

	if (octets < 4)
		return 0;

	memcpy(dst, tmp, INADDRSZ);
	return 1;
}