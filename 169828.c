passGetName(CharsString *passLine, int *passLinepos, CharsString *passHoldString,
		FileInfo *passNested, TranslationTableHeader *table) {
	TranslationTableCharacterAttributes attr;
	passHoldString->length = 0;
	do {
		attr = definedCharOrDots(passNested, passLine->chars[*passLinepos], 0,
				table)->attributes;
		if (passHoldString->length == 0) {
			if (!(attr & CTC_Letter)) {
				(*passLinepos)++;
				continue;
			}
		}
		if (!(attr & CTC_Letter)) break;
		passHoldString->chars[passHoldString->length++] = passLine->chars[*passLinepos];
		(*passLinepos)++;
	} while (*passLinepos < passLine->length);
	return 1;
}