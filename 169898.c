passGetVariableNumber(FileInfo *nested, CharsString *passLine, int *passLinepos,
		widechar *passHoldNumber) {
	if (!passGetNumber(passLine, passLinepos, passHoldNumber)) return 0;
	if ((*passHoldNumber >= 0) && (*passHoldNumber < NUMVAR)) return 1;
	compileError(nested, "variable number out of range");
	return 0;
}