gpg_ctx_free (struct _GpgCtx *gpg)
{
	gint i;

	if (gpg == NULL)
		return;

	if (gpg->session)
		g_object_unref (gpg->session);

	g_hash_table_foreach (gpg->userid_hint, userid_hint_free, NULL);
	g_hash_table_destroy (gpg->userid_hint);

	g_slist_free_full (gpg->userids, g_free);

	g_free (gpg->sigfile);

	if (gpg->recipients) {
		for (i = 0; i < gpg->recipients->len; i++)
			g_free (gpg->recipients->pdata[i]);

		g_ptr_array_free (gpg->recipients, TRUE);
	}

	if (gpg->stdin_fd != -1)
		close (gpg->stdin_fd);
	if (gpg->stdout_fd != -1)
		close (gpg->stdout_fd);
	if (gpg->stderr_fd != -1)
		close (gpg->stderr_fd);
	if (gpg->status_fd != -1)
		close (gpg->status_fd);
	if (gpg->passwd_fd != -1)
		close (gpg->passwd_fd);

	g_free (gpg->statusbuf);

	g_free (gpg->need_id);

	if (gpg->passwd) {
		memset (gpg->passwd, 0, strlen (gpg->passwd));
		g_free (gpg->passwd);
	}

	if (gpg->istream)
		g_object_unref (gpg->istream);

	if (gpg->ostream)
		g_object_unref (gpg->ostream);

	g_object_unref (gpg->diagnostics);

	if (gpg->signers)
		g_string_free (gpg->signers, TRUE);

	g_free (gpg);
}