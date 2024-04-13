static inline int process_named_entity_html(const char **buf, const char **start, size_t *length)
{
	*start = *buf;

	/* "&" is represented by a 0x26 in all supported encodings. That means
	 * the byte after represents a character or is the leading byte of an
	 * sequence of 8-bit code units. If in the ranges below, it represents
	 * necessarily a alpha character because none of the supported encodings
	 * has an overlap with ASCII in the leading byte (only on the second one) */
	while ((**buf >= 'a' && **buf <= 'z') ||
			(**buf >= 'A' && **buf <= 'Z') ||
			(**buf >= '0' && **buf <= '9')) {
		(*buf)++;
	}

	if (**buf != ';')
		return FAILURE;

	/* cast to size_t OK as the quantity is always non-negative */
	*length = *buf - *start;

	if (*length == 0)
		return FAILURE;

	return SUCCESS;
}