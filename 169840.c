getRuleDotsPattern(FileInfo *nested, CharsString *ruleDots, int *lastToken) {
	/* Interpret the dets operand */
	CharsString token;
	if (getToken(nested, &token, "Dots operand", lastToken)) {
		if (token.length == 1 && token.chars[0] == '=') {
			ruleDots->length = 0;
			return 1;
		}
		if (parseDots(nested, ruleDots, &token)) return 1;
	}
	return 0;
}