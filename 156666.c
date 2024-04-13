static GList *get_option_value_list(char *value, GDHCPOptionType type)
{
	char *pos = value;
	GList *list = NULL;

	if (!pos)
		return NULL;

	if (type == OPTION_STRING)
		return g_list_append(list, g_strdup(value));

	while ((pos = strchr(pos, ' '))) {
		*pos = '\0';

		list = g_list_append(list, g_strdup(value));

		value = ++pos;
	}

	list = g_list_append(list, g_strdup(value));

	return list;
}