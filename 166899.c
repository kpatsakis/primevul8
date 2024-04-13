proto_compare_name(gconstpointer p1_arg, gconstpointer p2_arg)
{
	const protocol_t *p1 = (const protocol_t *)p1_arg;
	const protocol_t *p2 = (const protocol_t *)p2_arg;

	return g_ascii_strcasecmp(p1->short_name, p2->short_name);
}