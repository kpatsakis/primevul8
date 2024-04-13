free_tablefiles(char **tables) {
	char **table;
	if (!tables) return;
	for (table = tables; *table; table++) free(*table);
	free(tables);
}