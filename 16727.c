next_token (const gchar *in,
            gchar **token)
{
	const gchar *start, *inptr = in;

	while (*inptr == ' ')
		inptr++;

	if (*inptr == '\0' || *inptr == '\n') {
		if (token)
			*token = NULL;
		return inptr;
	}

	start = inptr;
	while (*inptr && *inptr != ' ' && *inptr != '\n')
		inptr++;

	if (token)
		*token = g_strndup (start, inptr - start);

	return inptr;
}