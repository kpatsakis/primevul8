gpg_ctx_get_argv (struct _GpgCtx *gpg,
                  gint status_fd,
                  gchar **sfd,
                  gint passwd_fd,
                  gchar **pfd)
{
	const gchar *hash_str;
	GPtrArray *argv;
	gchar *buf;
	gint i;

	argv = g_ptr_array_new ();
	g_ptr_array_add (argv, (guint8 *) gpg_ctx_get_executable_name ());

	g_ptr_array_add (argv, (guint8 *) "--verbose");
	g_ptr_array_add (argv, (guint8 *) "--no-secmem-warning");
	g_ptr_array_add (argv, (guint8 *) "--no-greeting");
	g_ptr_array_add (argv, (guint8 *) "--no-tty");

	if (passwd_fd == -1) {
		/* only use batch mode if we don't intend on using the
		 * interactive --command-fd option */
		g_ptr_array_add (argv, (guint8 *) "--batch");
		g_ptr_array_add (argv, (guint8 *) "--yes");
	}

	*sfd = buf = g_strdup_printf ("--status-fd=%d", status_fd);
	g_ptr_array_add (argv, buf);

	if (passwd_fd != -1) {
		*pfd = buf = g_strdup_printf ("--command-fd=%d", passwd_fd);
		g_ptr_array_add (argv, buf);
	}

	switch (gpg->mode) {
	case GPG_CTX_MODE_SIGN:
		g_ptr_array_add (argv, (guint8 *) "--sign");
		g_ptr_array_add (argv, (guint8 *) "--detach");
		if (gpg->armor)
			g_ptr_array_add (argv, (guint8 *) "--armor");
		hash_str = gpg_hash_str (gpg->hash);
		if (hash_str)
			g_ptr_array_add (argv, (guint8 *) hash_str);
		if (gpg->userids) {
			GSList *uiter;

			for (uiter = gpg->userids; uiter; uiter = uiter->next) {
				g_ptr_array_add (argv, (guint8 *) "-u");
				g_ptr_array_add (argv, (guint8 *) uiter->data);
			}
		}
		g_ptr_array_add (argv, (guint8 *) "--output");
		g_ptr_array_add (argv, (guint8 *) "-");
		break;
	case GPG_CTX_MODE_VERIFY:
		if (!camel_session_get_online (gpg->session)) {
			/* this is a deprecated flag to gpg since 1.0.7 */
			/*g_ptr_array_add (argv, "--no-auto-key-retrieve");*/
			g_ptr_array_add (argv, (guint8 *) "--keyserver-options");
			g_ptr_array_add (argv, (guint8 *) "no-auto-key-retrieve");
		}
		g_ptr_array_add (argv, (guint8 *) "--verify");
		if (gpg->sigfile)
			g_ptr_array_add (argv, gpg->sigfile);
		g_ptr_array_add (argv, (guint8 *) "-");
		break;
	case GPG_CTX_MODE_ENCRYPT:
		g_ptr_array_add (argv, (guint8 *) "--encrypt");
		if (gpg->armor)
			g_ptr_array_add (argv, (guint8 *) "--armor");
		if (gpg->always_trust)
			g_ptr_array_add (argv, (guint8 *) "--always-trust");
		if (gpg->userids) {
			GSList *uiter;

			for (uiter = gpg->userids; uiter; uiter = uiter->next) {
				g_ptr_array_add (argv, (guint8 *) "-u");
				g_ptr_array_add (argv, (guint8 *) uiter->data);
			}
		}
		if (gpg->recipients) {
			for (i = 0; i < gpg->recipients->len; i++) {
				g_ptr_array_add (argv, (guint8 *) "-r");
				g_ptr_array_add (argv, gpg->recipients->pdata[i]);
			}
		}
		g_ptr_array_add (argv, (guint8 *) "--output");
		g_ptr_array_add (argv, (guint8 *) "-");
		break;
	case GPG_CTX_MODE_DECRYPT:
		g_ptr_array_add (argv, (guint8 *) "--decrypt");
		g_ptr_array_add (argv, (guint8 *) "--output");
		g_ptr_array_add (argv, (guint8 *) "-");
		break;
	case GPG_CTX_MODE_IMPORT:
		g_ptr_array_add (argv, (guint8 *) "--import");
		g_ptr_array_add (argv, (guint8 *) "-");
		break;
	case GPG_CTX_MODE_EXPORT:
		if (gpg->armor)
			g_ptr_array_add (argv, (guint8 *) "--armor");
		g_ptr_array_add (argv, (guint8 *) "--export");
		for (i = 0; i < gpg->recipients->len; i++)
			g_ptr_array_add (argv, gpg->recipients->pdata[i]);
		break;
	}

	g_ptr_array_add (argv, NULL);

	return argv;
}