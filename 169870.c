lou_compileString(const char *tableList, const char *inString) {
	int r;
	TranslationTableHeader *table = lou_getTable(tableList);
	if (!table) return 0;
	r = compileString(inString, &gCharacterClasses, &gCharacterClassAttribute,
			gOpcodeLengths, &gNewRuleOffset, &gNewRule, &gRuleNames, &table);
	gTable = table;
	return r;
}