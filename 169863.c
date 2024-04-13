getRuleCharsText(FileInfo *nested, CharsString *ruleChars, int *lastToken) {
	CharsString token;
	if (getToken(nested, &token, "Characters operand", lastToken))
		if (parseChars(nested, ruleChars, &token)) return 1;
	return 0;
}