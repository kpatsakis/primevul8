compileString(const char *inString, CharacterClass **characterClasses,
		TranslationTableCharacterAttributes *characterClassAttribute,
		short opcodeLengths[], TranslationTableOffset *newRuleOffset,
		TranslationTableRule **newRule, RuleName **ruleNames,
		TranslationTableHeader **table) {
	/* This function can be used to make changes to tables on the fly. */
	int k;
	FileInfo nested;
	if (inString == NULL) return 0;
	memset(&nested, 0, sizeof(nested));
	nested.fileName = inString;
	nested.encoding = noEncoding;
	nested.lineNumber = 1;
	nested.status = 0;
	nested.linepos = 0;
	for (k = 0; inString[k]; k++) nested.line[k] = inString[k];
	nested.line[k] = 0;
	nested.linelen = k;
	return compileRule(&nested, characterClasses, characterClassAttribute, opcodeLengths,
			newRuleOffset, newRule, ruleNames, table);
}