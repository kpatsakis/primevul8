hfinfo_char_value_format_display(int display, char buf[7], guint32 value)
{
	char *ptr = &buf[6];
	static const gchar hex_digits[16] =
	{ '0', '1', '2', '3', '4', '5', '6', '7',
	  '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

	*ptr = '\0';
	*(--ptr) = '\'';
	/* Properly format value */
	if (g_ascii_isprint(value)) {
		/*
		 * Printable, so just show the character, and, if it needs
		 * to be escaped, escape it.
		 */
		*(--ptr) = value;
		if (value == '\\' || value == '\'')
			*(--ptr) = '\\';
	} else {
		/*
		 * Non-printable; show it as an escape sequence.
		 */
		switch (value) {

		case '\0':
			/*
			 * Show a NUL with only one digit.
			 */
			*(--ptr) = '0';
			break;

		case '\a':
			*(--ptr) = 'a';
			break;

		case '\b':
			*(--ptr) = 'b';
			break;

		case '\f':
			*(--ptr) = 'f';
			break;

		case '\n':
			*(--ptr) = 'n';
			break;

		case '\r':
			*(--ptr) = 'r';
			break;

		case '\t':
			*(--ptr) = 't';
			break;

		case '\v':
			*(--ptr) = 'v';
			break;

		default:
			switch (FIELD_DISPLAY(display)) {

			case BASE_OCT:
				*(--ptr) = (value & 0x7) + '0';
				value >>= 3;
				*(--ptr) = (value & 0x7) + '0';
				value >>= 3;
				*(--ptr) = (value & 0x7) + '0';
				break;

			case BASE_HEX:
				*(--ptr) = hex_digits[value & 0x0F];
				value >>= 4;
				*(--ptr) = hex_digits[value & 0x0F];
				*(--ptr) = 'x';
				break;

			default:
				g_assert_not_reached();
			}
		}
		*(--ptr) = '\\';
	}
	*(--ptr) = '\'';
	return ptr;
}