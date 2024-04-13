passGetString(CharsString *passLine, int *passLinepos, CharsString *passHoldString,
		FileInfo *passNested) {
	passHoldString->length = 0;
	while (1) {
		if ((*passLinepos >= passLine->length) || !passLine->chars[*passLinepos]) {
			compileError(passNested, "unterminated string");
			return 0;
		}
		if (passLine->chars[*passLinepos] == 34) break;
		if (passLine->chars[*passLinepos] == QUOTESUB)
			passHoldString->chars[passHoldString->length++] = 34;
		else
			passHoldString->chars[passHoldString->length++] =
					passLine->chars[*passLinepos];
		(*passLinepos)++;
	}
	passHoldString->chars[passHoldString->length] = 0;
	(*passLinepos)++;
	return 1;
}