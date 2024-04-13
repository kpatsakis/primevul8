char **file_lines_parse(char *p, size_t size, int *numlines, TALLOC_CTX *mem_ctx)
{
	int i;
	char *s, **ret;

	if (!p) return NULL;

	for (s = p, i=0; s < p+size; s++) {
		if (s[0] == '\n') i++;
	}

	ret = talloc_array(mem_ctx, char *, i+2);
	if (!ret) {
		talloc_free(p);
		return NULL;
	}	
	
	talloc_steal(ret, p);
	
	memset(ret, 0, sizeof(ret[0])*(i+2));

	ret[0] = p;
	for (s = p, i=0; s < p+size; s++) {
		if (s[0] == '\n') {
			s[0] = 0;
			i++;
			ret[i] = s+1;
		}
		if (s[0] == '\r') s[0] = 0;
	}

	/* remove any blank lines at the end */
	while (i > 0 && ret[i-1][0] == 0) {
		i--;
	}

	if (numlines) *numlines = i;

	return ret;
}