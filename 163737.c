static inline char *find_param_list(char *path, size_t path_l, char delim)
{
	char *p;

	p = memchr(path, delim, path_l);
	return p ? p + 1 : NULL;
}