evbuffer_readline(struct evbuffer *buffer)
{
	u_char *data = EVBUFFER_DATA(buffer);
	size_t len = EVBUFFER_LENGTH(buffer);
	char *line;
	unsigned int i;

	for (i = 0; i < len; i++) {
		if (data[i] == '\r' || data[i] == '\n')
			break;
	}

	if (i == len)
		return (NULL);

	if ((line = malloc(i + 1)) == NULL) {
		fprintf(stderr, "%s: out of memory\n", __func__);
		return (NULL);
	}

	memcpy(line, data, i);
	line[i] = '\0';

	/*
	 * Some protocols terminate a line with '\r\n', so check for
	 * that, too.
	 */
	if ( i < len - 1 ) {
		char fch = data[i], sch = data[i+1];

		/* Drain one more character if needed */
		if ( (sch == '\r' || sch == '\n') && sch != fch )
			i += 1;
	}

	evbuffer_drain(buffer, i + 1);

	return (line);
}