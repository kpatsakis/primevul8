static int http_del_hdr_value(char *start, char *end, char **from, char *next)
{
	char *prev = *from;

	if (prev == start) {
		/* We're removing the first value. eat the semicolon, if <next>
		 * is lower than <end> */
		if (next < end)
			next++;

		while (next < end && HTTP_IS_SPHT(*next))
			next++;
	}
	else {
		/* Remove useless spaces before the old delimiter. */
		while (HTTP_IS_SPHT(*(prev-1)))
			prev--;
		*from = prev;

		/* copy the delimiter and if possible a space if we're
		 * not at the end of the line.
		 */
		if (next < end) {
			*prev++ = *next++;
			if (prev + 1 < next)
				*prev++ = ' ';
			while (next < end && HTTP_IS_SPHT(*next))
				next++;
		}
	}
	memmove(prev, next, end - next);
	return (prev - next);
}