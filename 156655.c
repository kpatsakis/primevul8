static GList *get_domains(int maxlen, unsigned char *value)

{
	GList *list = NULL;
	int pos = 0;
	unsigned char *c;
	char dns_name[NS_MAXDNAME + 1];

	if (!value || maxlen < 3)
		return NULL;

	while (pos < maxlen) {
		strncpy(dns_name, (char *)&value[pos], NS_MAXDNAME);

		c = (unsigned char *)dns_name;
		while (c && *c) {
			int jump;
			jump = *c;
			*c = '.';
			c += jump + 1;
		}
		list = g_list_prepend(list, g_strdup(&dns_name[1]));
		pos += (char *)c - dns_name + 1;
	}

	return g_list_reverse(list);
}