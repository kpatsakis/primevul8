reserveSpaceInTable(FileInfo *nested, int count, TranslationTableHeader **table) {
	return (allocateSpaceInTable(nested, NULL, count, table));
}