get_path_component(char *name, size_t n, const char *fn)
{
	char *p;
	size_t l;

	p = strchr(fn, '/');
	if (p == NULL) {
		if ((l = strlen(fn)) == 0)
			return (0);
	} else
		l = p - fn;
	if (l > n -1)
		return (-1);
	memcpy(name, fn, l);
	name[l] = '\0';

	return ((int)l);
}