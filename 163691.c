int del_hdr_value(struct buffer *buf, char **from, char *next)
{
	char *prev = *from;

	if (*prev == ':') {
		/* We're removing the first value, preserve the colon and add a
		 * space if possible.
		 */
		if (!http_is_crlf[(unsigned char)*next])
			next++;
		prev++;
		if (prev < next)
			*prev++ = ' ';

		while (http_is_spht[(unsigned char)*next])
			next++;
	} else {
		/* Remove useless spaces before the old delimiter. */
		while (http_is_spht[(unsigned char)*(prev-1)])
			prev--;
		*from = prev;

		/* copy the delimiter and if possible a space if we're
		 * not at the end of the line.
		 */
		if (!http_is_crlf[(unsigned char)*next]) {
			*prev++ = *next++;
			if (prev + 1 < next)
				*prev++ = ' ';
			while (http_is_spht[(unsigned char)*next])
				next++;
		}
	}
	return buffer_replace2(buf, prev, next, NULL, 0);
}