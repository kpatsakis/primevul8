gpg_ctx_get_executable_name (void)
{
	static gint index = -1;
	const gchar *names[] = {
		"gpg",
		"gpg2",
		NULL
	};

	if (index == -1) {
		for (index = 0; names[index]; index++) {
			gchar *path = g_find_program_in_path (names[index]);

			if (path) {
				g_free (path);
				break;
			}
		}

		if (!names[index])
			index = 0;
	}

	return names[index];
}