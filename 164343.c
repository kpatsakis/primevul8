static int is_fullwidth(int c)
{
	int i;

	if (c < mbfl_eaw_table[0].begin) {
		return 0;
	}

	for (i = 0; i < sizeof(mbfl_eaw_table) / sizeof(mbfl_eaw_table[0]); i++) {
		if (mbfl_eaw_table[i].begin <= c && c <= mbfl_eaw_table[i].end) {
			return 1;
		}
	}

	return 0;
}