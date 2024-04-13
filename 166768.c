prefix_hash (gconstpointer key) {
	/* end the string at the dot and compute its hash */
	gchar* copy = g_strdup((const gchar *)key);
	gchar* c    = copy;
	guint tmp;

	for (; *c; c++) {
		if (*c == '.') {
			*c = 0;
			break;
		}
	}

	tmp = g_str_hash(copy);
	g_free(copy);
	return tmp;
}