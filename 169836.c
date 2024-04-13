passInsertAttributes(CharsString *passLine, int *passLinepos, int *passPrevLinepos,
		CharsString *passHoldString, TranslationTableCharacterAttributes *passAttributes,
		FileInfo *passNested, widechar *passInstructions, int *passIC,
		TranslationTableHeader *table) {
	passInstructions[(*passIC)++] = pass_attributes;
	passInstructions[(*passIC)++] = *passAttributes >> 16;
	passInstructions[(*passIC)++] = *passAttributes & 0xffff;
	if (!passGetRange(passLine, passLinepos, passPrevLinepos, passHoldString, passNested,
				passInstructions, passIC, table))
		return 0;
	return 1;
}