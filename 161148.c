idr_relaxed_filenames(char *map)
{
	int i;

	for (i = 0x21; i <= 0x2F; i++)
		map[i] = 1;
	for (i = 0x3A; i <= 0x41; i++)
		map[i] = 1;
	for (i = 0x5B; i <= 0x5E; i++)
		map[i] = 1;
	map[0x60] = 1;
	for (i = 0x7B; i <= 0x7E; i++)
		map[i] = 1;
}