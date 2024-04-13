compileCharDef(FileInfo *nested, TranslationTableOpcode opcode,
		TranslationTableCharacterAttributes attributes, int *lastToken,
		TranslationTableOffset *newRuleOffset, TranslationTableRule **newRule, int noback,
		int nofor, TranslationTableHeader **table) {
	CharsString ruleChars;
	CharsString ruleDots;
	TranslationTableCharacter *character;
	TranslationTableCharacter *cell = NULL;
	int k;
	if (!getRuleCharsText(nested, &ruleChars, lastToken)) return 0;
	if (!getRuleDotsPattern(nested, &ruleDots, lastToken)) return 0;
	if (ruleChars.length != 1) {
		compileError(nested, "Exactly one character is required.");
		return 0;
	}
	if (ruleDots.length < 1) {
		compileError(nested, "At least one cell is required.");
		return 0;
	}
	if (attributes & (CTC_UpperCase | CTC_LowerCase)) attributes |= CTC_Letter;
	character = addCharOrDots(nested, ruleChars.chars[0], 0, table);
	character->attributes |= attributes;
	character->uppercase = character->lowercase = character->realchar;
	for (k = ruleDots.length - 1; k >= 0; k -= 1) {
		cell = compile_findCharOrDots(ruleDots.chars[k], 1, *table);
		if (!cell) {
			cell = addCharOrDots(nested, ruleDots.chars[k], 1, table);
			cell->uppercase = cell->lowercase = cell->realchar;
		}
	}
	if (ruleDots.length == 1) {
		cell->attributes |= attributes;
		putCharAndDots(nested, ruleChars.chars[0], ruleDots.chars[0], table);
	}
	if (!addRule(nested, opcode, &ruleChars, &ruleDots, 0, 0, newRuleOffset, newRule,
				noback, nofor, table))
		return 0;
	return 1;
}