gpg_import_keys_sync (CamelCipherContext *context,
                      CamelStream *istream,
                      GCancellable *cancellable,
                      GError **error)
{
	struct _GpgCtx *gpg;
	gboolean success = FALSE;

	gpg = gpg_ctx_new (context);
	gpg_ctx_set_mode (gpg, GPG_CTX_MODE_IMPORT);
	gpg_ctx_set_istream (gpg, istream);

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