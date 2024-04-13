passIsNumber(CharsString *passLine, int *passLinepos, int *passPrevLinepos,
		CharsString *passHoldString, widechar *passHoldNumber, FileInfo *passNested,
		TranslationTableHeader *table) {
	pass_Codes passCode = passGetScriptToken(passLine, passLinepos, passPrevLinepos,
			passHoldString, passHoldNumber, passNested, table);
	if (passCode != pass_numberFound) {
		compileError(passNested, "a number expected");
		return 0;
	}
	return 1;
}