userid_hint_free (gpointer key,
                  gpointer value,
                  gpointer user_data)
{
	g_free (key);
	g_free (value);
}