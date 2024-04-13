definedCharOrDots(FileInfo *nested, widechar c, int m, TranslationTableHeader *table) {
	static TranslationTableCharacter noChar = {
		.next = 0,
		.definitionRule = 0,
		.otherRules = 0,
		.attributes = CTC_Space,
		.realchar = 32,
		.uppercase = 32,
		.lowercase = 32,
	};
	static TranslationTableCharacter noDots = {
		.next = 0,
		.definitionRule = 0,
		.otherRules = 0,
		.attributes = CTC_Space,
		.realchar = B16,
		.uppercase = B16,
		.lowercase = B16,
	};
	TranslationTableCharacter *notFound;
	TranslationTableCharacter *charOrDots = compile_findCharOrDots(c, m, table);
	if (charOrDots) return charOrDots;
	if (m == 0) {
		notFound = &noChar;
		compileError(nested, "character %s should be defined at this point but is not",
				_lou_showString(&c, 1));
	} else {
		notFound = &noDots;
		compileError(nested, "cell %s should be defined at this point but is not",
				unknownDots(c));
	}
	return notFound;
}