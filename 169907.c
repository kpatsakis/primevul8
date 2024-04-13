passGetRange(CharsString *passLine, int *passLinepos, int *passPrevLinepos,
		CharsString *passHoldString, FileInfo *passNested, widechar *passInstructions,
		int *passIC, TranslationTableHeader *table) {
	widechar passHoldNumber;
	pass_Codes passCode = passGetScriptToken(passLine, passLinepos, passPrevLinepos,
			passHoldString, &passHoldNumber, passNested, table);
	if (!(passCode == pass_comma || passCode == pass_rightParen)) {
		compileError(passNested, "invalid range");
		return 0;
	}
	if (passCode == pass_rightParen) {
		passInstructions[(*passIC)++] = 1;
		passInstructions[(*passIC)++] = 1;
		return 1;
	}
	if (!passIsNumber(passLine, passLinepos, passPrevLinepos, passHoldString,
				&passHoldNumber, passNested, table))
		return 0;
	passInstructions[(*passIC)++] = passHoldNumber;
	passCode = passGetScriptToken(passLine, passLinepos, passPrevLinepos, passHoldString,
			&passHoldNumber, passNested, table);
	if (!(passCode == pass_comma || passCode == pass_rightParen)) {
		compileError(passNested, "invalid range");
		return 0;
	}
	if (passCode == pass_rightParen) {
		passInstructions[(*passIC)++] = passHoldNumber;
		return 1;
	}
	if (!passIsNumber(passLine, passLinepos, passPrevLinepos, passHoldString,
				&passHoldNumber, passNested, table))
		return 0;
	passInstructions[(*passIC)++] = passHoldNumber;
	if (!passIsRightParen(passLine, passLinepos, passPrevLinepos, passHoldString,
				passNested, table))
		return 0;
	return 1;
}