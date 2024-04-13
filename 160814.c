static int parse_ip_val_buffer(char *in_buf, int *offset,
				char *out_buf, int out_len)
{
	char *x;
	char *start;

	/*
	 * in_buf has sequence of characters that are seperated by
	 * the character ';'. The last sequence does not have the
	 * terminating ";" character.
	 */
	start = in_buf + *offset;

	x = strchr(start, ';');
	if (x)
		*x = 0;
	else
		x = start + strlen(start);

	if (strlen(start) != 0) {
		int i = 0;
		/*
		 * Get rid of leading spaces.
		 */
		while (start[i] == ' ')
			i++;

		if ((x - start) <= out_len) {
			strcpy(out_buf, (start + i));
			*offset += (x - start) + 1;
			return 1;
		}
	}
	return 0;
}