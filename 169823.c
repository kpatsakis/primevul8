lou_getEmphClasses(const char *tableList) {
	const char *names[MAX_EMPH_CLASSES + 1];
	unsigned int count = 0;
	const TranslationTableHeader *table = lou_getTable(tableList);
	if (!table) return NULL;

	while (count < MAX_EMPH_CLASSES) {
		char const *name = table->emphClasses[count];
		if (!name) break;
		names[count++] = name;
	}
	names[count++] = NULL;

	{
		unsigned int size = count * sizeof(names[0]);
		char const **result = malloc(size);
		if (!result) return NULL;
		/* The void* cast is necessary to stop MSVC from warning about
		 * different 'const' qualifiers (C4090). */
		memcpy((void *)result, names, size);
		return result;
	}
}