_lou_resolveTable(const char *tableList, const char *base) {
	char **tableFiles = (*tableResolver)(tableList, base);
	char **result = copyStringArray(tableFiles);
	if (tableResolver == &_lou_defaultTableResolver) free_tablefiles(tableFiles);
	return result;
}