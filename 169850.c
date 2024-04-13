compileBeforeAfter(FileInfo *nested, int *lastToken) {
	/* 1=before, 2=after, 0=error */
	CharsString token;
	CharsString tmp;
	if (getToken(nested, &token, "last word before or after", lastToken))
		if (parseChars(nested, &tmp, &token)) {
			if (eqasc2uni((unsigned char *)"before", tmp.chars, 6)) return 1;
			if (eqasc2uni((unsigned char *)"after", tmp.chars, 5)) return 2;
		}
	return 0;
}