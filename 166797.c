other_decode_bitfield_varint_value(char *buf, guint64 val, guint64 mask, const int width)
{
	int i = 0;
	guint64 bit;
	char *p;

	p = buf;
	bit = G_GUINT64_CONSTANT(1) << (width - 1);
	for (;;) {
		if (((8-(i % 8)) != 8) && /* MSB is never used for value. */
			(mask & bit)) {
			/* This bit is part of the field.  Show its value. */
			if (val & bit)
				*p++ = '1';
			else
				*p++ = '0';
		} else {
			/* This bit is not part of the field. */
			*p++ = '.';
		}
		bit >>= 1;
		i++;
		if (i >= width)
			break;
		if (i % 4 == 0)
			*p++ = ' ';
	}

	*p = '\0';
	return p;
}