proto_check_field_name(const gchar *field_name)
{
	const char *p = field_name;
	guchar c = '.', lastc;

	do {
		lastc = c;
		c = *(p++);
		/* Leading '.' or substring ".." are disallowed. */
		if (c == '.' && lastc == '.') {
			break;
		}
	} while (fld_abbrev_chars[c]);

	/* Trailing '.' is disallowed. */
	if (lastc == '.') {
		return '.';
	}
	return c;
}