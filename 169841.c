getToken(FileInfo *nested, CharsString *result, const char *description, int *lastToken) {
	/* Find the next string of contiguous non-whitespace characters. If this
	 * is the last token on the line, return 2 instead of 1. */
	while (!atEndOfLine(nested) && atTokenDelimiter(nested)) nested->linepos++;
	result->length = 0;
	while (!atEndOfLine(nested) && !atTokenDelimiter(nested)) {
		int maxlen = MAXSTRING;
		if (result->length >= maxlen) {
			compileError(nested, "more than %d characters (bytes)", maxlen);
			return 0;
		} else
			result->chars[result->length++] = nested->line[nested->linepos++];
	}
	if (!result->length) {
		/* Not enough tokens */
		if (description) compileError(nested, "%s not specified.", description);
		return 0;
	}
	result->chars[result->length] = 0;
	while (!atEndOfLine(nested) && atTokenDelimiter(nested)) nested->linepos++;
	return (*lastToken = atEndOfLine(nested)) ? 2 : 1;
}