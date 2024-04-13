static void remove_option_value(gpointer data)
{
	GList *option_value = data;

	g_list_foreach(option_value, remove_value, NULL);
	g_list_free(option_value);
}