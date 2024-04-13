static int acl_parse_meth(const char **text, struct acl_pattern *pattern, int *opaque, char **err)
{
	int len, meth;

	len  = strlen(*text);
	meth = find_http_meth(*text, len);

	pattern->val.i = meth;
	if (meth == HTTP_METH_OTHER) {
		pattern->ptr.str = strdup(*text);
		if (!pattern->ptr.str) {
			memprintf(err, "out of memory while loading pattern");
			return 0;
		}
		pattern->len = len;
	}
	return 1;
}