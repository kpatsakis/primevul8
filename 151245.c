void parse_sa_devices(char *argv, struct activity *a, int max_len, int *opt, int pos)
{
	char *t;

	for (t = strtok(argv + pos, ","); t; t = strtok(NULL, ",")) {
		a->item_list_sz += add_list_item(&(a->item_list), t, max_len);
	}
	if (a->item_list_sz) {
		a->options |= AO_LIST_ON_CMDLINE;
	}
	(*opt)++;
}