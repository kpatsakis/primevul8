passIsRightParen(CharsString *passLine, int *passLinepos, int *passPrevLinepos,
		CharsString *passHoldString, FileInfo *passNested,
		TranslationTableHeader *table) {
	widechar passHoldNumber;
	pass_Codes passCode = passGetScriptToken(passLine, passLinepos, passPrevLinepos,
			passHoldString, &passHoldNumber, passNested, table);
	if (passCode != pass_rightParen) {
		compileError(passNested, "')' expected");
		return 0;
	}
	return 1;
}