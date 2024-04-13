static gchar *convert_to_hex(unsigned char *buf, int len)
{
	gchar *ret = g_try_malloc(len * 2 + 1);
	int i;

	for (i = 0; ret && i < len; i++)
		g_snprintf(ret + i * 2, 3, "%02x", buf[i]);

	return ret;
}