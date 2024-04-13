device_list_uuids (const gchar * const *uuids)
{
	GPtrArray *ret;
	guint i;

	if (uuids == NULL)
		return NULL;

	ret = g_ptr_array_new ();

	for (i = 0; uuids[i] != NULL; i++) {
		const char *uuid;

		uuid = bluetooth_uuid_to_string (uuids[i]);
		if (uuid == NULL)
			continue;
		g_ptr_array_add (ret, g_strdup (uuid));
	}

	if (ret->len == 0) {
		g_ptr_array_free (ret, TRUE);
		return NULL;
	}

	g_ptr_array_add (ret, NULL);

	return (char **) g_ptr_array_free (ret, FALSE);
}