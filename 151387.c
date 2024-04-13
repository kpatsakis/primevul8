evbuffer_readln(struct evbuffer *buffer, size_t *n_read_out,
		enum evbuffer_eol_style eol_style)
{
	u_char *data = EVBUFFER_DATA(buffer);
	u_char *start_of_eol, *end_of_eol;
	size_t len = EVBUFFER_LENGTH(buffer);
	char *line;
	unsigned int i, n_to_copy, n_to_drain;

	if (n_read_out)
		*n_read_out = 0;

	/* depending on eol_style, set start_of_eol to the first character
	 * in the newline, and end_of_eol to one after the last character. */
	switch (eol_style) {
	case EVBUFFER_EOL_ANY:
		for (i = 0; i < len; i++) {
			if (data[i] == '\r' || data[i] == '\n')
				break;
		}
		if (i == len)
			return (NULL);
		start_of_eol = data+i;
		++i;
		for ( ; i < len; i++) {
			if (data[i] != '\r' && data[i] != '\n')
				break;
		}
		end_of_eol = data+i;
		break;
	case EVBUFFER_EOL_CRLF:
		end_of_eol = memchr(data, '\n', len);
		if (!end_of_eol)
			return (NULL);
		if (end_of_eol > data && *(end_of_eol-1) == '\r')
			start_of_eol = end_of_eol - 1;
		else
			start_of_eol = end_of_eol;
		end_of_eol++; /*point to one after the LF. */
		break;
	case EVBUFFER_EOL_CRLF_STRICT: {
		u_char *cp = data;
		while ((cp = memchr(cp, '\r', len-(cp-data)))) {
			if (cp < data+len-1 && *(cp+1) == '\n')
				break;
			if (++cp >= data+len) {
				cp = NULL;
				break;
			}
		}
		if (!cp)
			return (NULL);
		start_of_eol = cp;
		end_of_eol = cp+2;
		break;
	}
	case EVBUFFER_EOL_LF:
		start_of_eol = memchr(data, '\n', len);
		if (!start_of_eol)
			return (NULL);
		end_of_eol = start_of_eol + 1;
		break;
	default:
		return (NULL);
	}

	n_to_copy = start_of_eol - data;
	n_to_drain = end_of_eol - data;

	if ((line = malloc(n_to_copy+1)) == NULL) {
		event_warn("%s: out of memory\n", __func__);
		return (NULL);
	}

	memcpy(line, data, n_to_copy);
	line[n_to_copy] = '\0';

	evbuffer_drain(buffer, n_to_drain);
	if (n_read_out)
		*n_read_out = (size_t)n_to_copy;

	return (line);
}