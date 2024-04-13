gpg_ctx_set_userid (struct _GpgCtx *gpg,
                    const gchar *userid)
{
	g_slist_free_full (gpg->userids, g_free);
	gpg->userids = NULL;

	if (userid && *userid) {
		gchar **uids = g_strsplit (userid, " ", -1);

		if (!uids) {
			gpg->userids = g_slist_append (gpg->userids, g_strdup (userid));
		} else {
			gint ii;

			for (ii = 0; uids[ii]; ii++) {
				const gchar *uid = uids[ii];

				if (*uid) {
					gpg->userids = g_slist_append (gpg->userids, g_strdup (uid));
				}
			}

			g_strfreev (uids);
		}
	}
}