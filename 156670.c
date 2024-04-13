static GDHCPIAPrefix *copy_prefix(gpointer data)
{
	GDHCPIAPrefix *copy, *prefix = data;

	copy = g_try_new(GDHCPIAPrefix, 1);
	if (!copy)
		return NULL;

	memcpy(copy, prefix, sizeof(GDHCPIAPrefix));

	return copy;
}