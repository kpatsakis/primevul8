isoent_find_entry(struct isoent *rootent, const char *fn)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
	char name[_MAX_FNAME];/* Included null terminator size. */
#elif defined(NAME_MAX) && NAME_MAX >= 255
	char name[NAME_MAX+1];
#else
	char name[256];
#endif
	struct isoent *isoent, *np;
	int l;

	isoent = rootent;
	np = NULL;
	for (;;) {
		l = get_path_component(name, sizeof(name), fn);
		if (l == 0)
			break;
		fn += l;
		if (fn[0] == '/')
			fn++;

		np = isoent_find_child(isoent, name);
		if (np == NULL)
			break;
		if (fn[0] == '\0')
			break;/* We found out the entry */

		/* Try sub directory. */
		isoent = np;
		np = NULL;
		if (!isoent->dir)
			break;/* Not directory */
	}

	return (np);
}