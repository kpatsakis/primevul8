compileTranslationTable(const char *tableList, CharacterClass **characterClasses,
		TranslationTableCharacterAttributes *characterClassAttribute,
		short opcodeLengths[], TranslationTableOffset *newRuleOffset,
		TranslationTableRule **newRule, RuleName **ruleNames) {
	TranslationTableHeader *table = NULL;
	char **tableFiles;
	char **subTable;
	errorCount = warningCount = fileCount = 0;
	*characterClasses = NULL;
	*ruleNames = NULL;
	if (tableList == NULL) return NULL;
	if (!opcodeLengths[0]) {
		TranslationTableOpcode opcode;
		for (opcode = 0; opcode < CTO_None; opcode++)
			opcodeLengths[opcode] = (short)strlen(opcodeNames[opcode]);
	}
	allocateHeader(NULL, &table);

	/* Initialize emphClasses array */
	table->emphClasses[0] = NULL;

	/* Compile things that are necesary for the proper operation of
	 * liblouis or liblouisxml or liblouisutdml */
	compileString("space \\s 0", characterClasses, characterClassAttribute, opcodeLengths,
			newRuleOffset, newRule, ruleNames, &table);
	compileString("noback sign \\x0000 0", characterClasses, characterClassAttribute,
			opcodeLengths, newRuleOffset, newRule, ruleNames, &table);
	compileString("space \\x00a0 a unbreakable space", characterClasses,
			characterClassAttribute, opcodeLengths, newRuleOffset, newRule, ruleNames,
			&table);
	compileString("space \\x001b 1b escape", characterClasses, characterClassAttribute,
			opcodeLengths, newRuleOffset, newRule, ruleNames, &table);
	compileString("space \\xffff 123456789abcdef ENDSEGMENT", characterClasses,
			characterClassAttribute, opcodeLengths, newRuleOffset, newRule, ruleNames,
			&table);

	/* Compile all subtables in the list */
	if (!(tableFiles = _lou_resolveTable(tableList, NULL))) {
		errorCount++;
		goto cleanup;
	}
	for (subTable = tableFiles; *subTable; subTable++)
		if (!compileFile(*subTable, characterClasses, characterClassAttribute,
					opcodeLengths, newRuleOffset, newRule, ruleNames, &table))
			goto cleanup;

/* Clean up after compiling files */
cleanup:
	free_tablefiles(tableFiles);
	if (*characterClasses) deallocateCharacterClasses(characterClasses);
	if (*ruleNames) deallocateRuleNames(ruleNames);
	if (warningCount) _lou_logMessage(LOG_WARN, "%d warnings issued", warningCount);
	if (!errorCount) {
		setDefaults(table);
		table->tableSize = tableSize;
		table->bytesUsed = tableUsed;
	} else {
		_lou_logMessage(LOG_ERROR, "%d errors found.", errorCount);
		if (table) free(table);
		table = NULL;
	}
	return table;
}