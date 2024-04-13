bluetooth_client_get_connectable(const char **uuids)
{
	int i, j;

	for (i = 0; uuids && uuids[i] != NULL; i++) {
		for (j = 0; j < G_N_ELEMENTS (connectable_uuids); j++) {
			if (g_str_equal (connectable_uuids[j], uuids[i]))
				return TRUE;
		}
	}

	return FALSE;
}