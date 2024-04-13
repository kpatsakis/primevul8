getRuleDotsText(FileInfo *nested, CharsString *ruleDots, int *lastToken) {
	CharsString token;
	if (getToken(nested, &token, "characters", lastToken))
		if (parseChars(nested, ruleDots, &token)) return 1;
	return 0;
}