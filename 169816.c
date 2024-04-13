compileFile(const char *fileName, CharacterClass **characterClasses,
		TranslationTableCharacterAttributes *characterClassAttribute,
		short opcodeLengths[], TranslationTableOffset *newRuleOffset,
		TranslationTableRule **newRule, RuleName **ruleNames,
		TranslationTableHeader **table) {
	FileInfo nested;
	fileCount++;
	nested.fileName = fileName;
	nested.encoding = noEncoding;
	nested.status = 0;
	nested.lineNumber = 0;
	if ((nested.in = fopen(nested.fileName, "rb"))) {
		while (_lou_getALine(&nested))
			compileRule(&nested, characterClasses, characterClassAttribute, opcodeLengths,
					newRuleOffset, newRule, ruleNames, table);
		fclose(nested.in);
		return 1;
	} else
		_lou_logMessage(LOG_ERROR, "Cannot open table '%s'", nested.fileName);
	errorCount++;
	return 0;
}