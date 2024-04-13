protoo_strlcpy(gchar *dest, const gchar *src, gsize dest_size)
{
	gsize res = g_strlcpy(dest, src, dest_size);

	if (res > dest_size)
		res = dest_size;
	return (int) res;
}