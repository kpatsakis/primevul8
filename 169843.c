_lou_findOpcodeNumber(const char *toFind) {
	/* Used by tools such as lou_debug */
	static TranslationTableOpcode lastOpcode = 0;
	TranslationTableOpcode opcode = lastOpcode;
	int length = (int)strlen(toFind);
	do {
		if (length == gOpcodeLengths[opcode] &&
				strcasecmp(toFind, opcodeNames[opcode]) == 0) {
			lastOpcode = opcode;
			return opcode;
		}
		opcode++;
		if (opcode >= CTO_None) opcode = 0;
	} while (opcode != lastOpcode);
	return CTO_None;
}