lou_getTypeformForEmphClass(const char *tableList, const char *emphClass) {
	int i;
	TranslationTableHeader *table = lou_getTable(tableList);
	if (!table) return 0;
	for (i = 0; table->emphClasses[i]; i++)
		if (strcmp(emphClass, table->emphClasses[i]) == 0) return italic << i;
	return 0;
}