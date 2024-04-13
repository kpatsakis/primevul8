static inline void debug(GDHCPClient *client, const char *format, ...)
{
	char str[256];
	va_list ap;

	if (!client->debug_func)
		return;

	va_start(ap, format);

	if (vsnprintf(str, sizeof(str), format, ap) > 0)
		client->debug_func(str, client->debug_data);

	va_end(ap);
}