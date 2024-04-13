cdf_find_stream(const cdf_dir_t *dir, const char *name, int type)
{
	size_t i, name_len = strlen(name) + 1;

	for (i = dir->dir_len; i > 0; i--)
		if (dir->dir_tab[i - 1].d_type == type &&
		    cdf_namecmp(name, dir->dir_tab[i - 1].d_name, name_len)
		    == 0)
			break;
	if (i > 0)
		return CAST(int, i);

	DPRINTF(("Cannot find type %d `%s'\n", type, name));
	errno = ESRCH;
	return 0;
}