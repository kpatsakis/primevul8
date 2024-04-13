verifyStringOrDots(FileInfo *nested, TranslationTableOpcode opcode, int isString,
		int actionPart, int nofor) {
	if (!wantsString(opcode, actionPart, nofor) == !isString) return 1;

	compileError(nested, "%s are not allowed in the %s part of a %s translation %s rule.",
			isString ? "strings" : "dots", getPartName(actionPart),
			nofor ? "backward" : "forward", _lou_findOpcodeName(opcode));

	return 0;
}