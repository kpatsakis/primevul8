int count_csval_arg(char *arg_v)
{
	int nr = 0;
	char *t;

	if (arg_v[0] == '\0')
		return 0;

	if (strchr(arg_v, ',')) {
		for (t = arg_v; t; t = strchr(t + 1, ',')) {
			nr++;
		}
	}
	if (!nr) {
		nr = 1;
	}

	return nr;
}