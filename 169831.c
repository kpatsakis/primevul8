lou_getTable(const char *tableList) {
	/* Keep track of which tables have already been compiled */
	int tableListLen;
	ChainEntry *currentEntry = NULL;
	ChainEntry *lastEntry = NULL;
	TranslationTableHeader *newTable;
	if (tableList == NULL || *tableList == 0) return NULL;
	errorCount = fileCount = 0;
	tableListLen = (int)strlen(tableList);
	/* See if this is the last table used. */
	if (lastTrans != NULL)
		if (tableListLen == lastTrans->tableListLength &&
				(memcmp(&lastTrans->tableList[0], tableList, tableListLen)) == 0)
			return (gTable = lastTrans->table);
	/* See if Table has already been compiled */
	currentEntry = tableChain;
	while (currentEntry != NULL) {
		if (tableListLen == currentEntry->tableListLength &&
				(memcmp(&currentEntry->tableList[0], tableList, tableListLen)) == 0) {
			lastTrans = currentEntry;
			return (gTable = currentEntry->table);
		}
		lastEntry = currentEntry;
		currentEntry = currentEntry->next;
	}
	if ((newTable = compileTranslationTable(tableList, &gCharacterClasses,
				 &gCharacterClassAttribute, gOpcodeLengths, &gNewRuleOffset, &gNewRule,
				 &gRuleNames))) {
		/* Add a new entry to the table chain. */
		int entrySize = sizeof(ChainEntry) + tableListLen;
		ChainEntry *newEntry = malloc(entrySize);
		if (!newEntry) _lou_outOfMemory();
		if (tableChain == NULL)
			tableChain = newEntry;
		else
			lastEntry->next = newEntry;
		newEntry->next = NULL;
		newEntry->table = newTable;
		newEntry->tableListLength = tableListLen;
		memcpy(&newEntry->tableList[0], tableList, tableListLen);
		lastTrans = newEntry;
		return (gTable = newEntry->table);
	}
	_lou_logMessage(LOG_ERROR, "%s could not be compiled", tableList);
	return NULL;
}