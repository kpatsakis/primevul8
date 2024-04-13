wantsString(TranslationTableOpcode opcode, int actionPart, int nofor) {
	if (opcode == CTO_Correct) return 1;
	if (opcode != CTO_Context) return 0;
	return !nofor == !actionPart;
}