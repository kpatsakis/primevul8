GSList *g_dhcpv6_copy_prefixes(GSList *prefixes)
{
	GSList *copy = NULL;
	GSList *list;

	for (list = prefixes; list; list = list->next)
		copy = g_slist_prepend(copy, copy_prefix(list->data));

	return copy;
}