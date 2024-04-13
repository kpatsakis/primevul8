gpg_export_keys_sync (CamelCipherContext *context,
                      GPtrArray *keys,
                      CamelStream *ostream,
                      GCancellable *cancellable,
                      GError **error)
{
	struct _GpgCtx *gpg;
	gboolean success = FALSE;
	gint i;

	gpg = gpg_ctx_new (context);
	gpg_ctx_set_mode (gpg, GPG_CTX_MODE_EXPORT);
	gpg_ctx_set_armor (gpg, TRUE);
	gpg_ctx_set_ostream (gpg, ostream);

	for (i = 0; i < keys->len; i++) {
		gpg_ctx_add_recipient (gpg, keys->pdata[i]);
	}

	if (!gpg_ctx_op_start (gpg, error))
		goto fail;

	while (!gpg_ctx_op_complete (gpg)) {
		if (gpg_ctx_op_step (gpg, cancellable, error) == -1) {
			gpg_ctx_op_cancel (gpg);
			goto fail;
		}
	}

	if (gpg_ctx_op_wait (gpg) != 0) {
		const gchar *diagnostics;

		diagnostics = gpg_ctx_get_diagnostics (gpg);
		g_set_error (
			error, CAMEL_ERROR, CAMEL_ERROR_GENERIC, "%s",
			(diagnostics != NULL && *diagnostics != '\0') ?
			diagnostics : _("Failed to execute gpg."));
		goto fail;
	}

	success = TRUE;
fail:
	gpg_ctx_free (gpg);

	return success;
}