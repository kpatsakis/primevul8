static void remove_value(gpointer data, gpointer user_data)
{
	char *value = data;
	g_free(value);
}