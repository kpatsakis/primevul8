getCharacters(FileInfo *nested, CharsString *characters, int *lastToken) {
	/* Get ruleChars string */
	CharsString token;
	if (getToken(nested, &token, "characters", lastToken))
		if (parseChars(nested, characters, &token)) return 1;
	return 0;
}