static int acl_parse_ver(const char **text, struct acl_pattern *pattern, int *opaque, char **err)
{
	pattern->ptr.str = strdup(*text);
	if (!pattern->ptr.str) {
		memprintf(err, "out of memory while loading pattern");
		return 0;
	}
	pattern->len = strlen(*text);
	return 1;
}