gpg_encrypt_sync (CamelCipherContext *context,
                  const gchar *userid,
                  GPtrArray *recipients,
                  CamelMimePart *ipart,
                  CamelMimePart *opart,
                  GCancellable *cancellable,
                  GError **error)
{
	CamelCipherContextClass *class;
	CamelGpgContext *ctx = (CamelGpgContext *) context;
	struct _GpgCtx *gpg;
	CamelStream *istream, *ostream, *vstream;
	CamelMimePart *encpart, *verpart;
	CamelDataWrapper *dw;
	CamelContentType *ct;
	CamelMultipartEncrypted *mpe;
	gboolean success = FALSE;
	gint i;

	class = CAMEL_CIPHER_CONTEXT_GET_CLASS (context);

	ostream = camel_stream_mem_new ();
	istream = camel_stream_mem_new ();
	if (camel_cipher_canonical_to_stream (
		ipart, CAMEL_MIME_FILTER_CANON_CRLF, istream, NULL, error) == -1) {
		g_prefix_error (
			error, _("Could not generate encrypting data: "));
		goto fail1;
	}

	gpg = gpg_ctx_new (context);
	gpg_ctx_set_mode (gpg, GPG_CTX_MODE_ENCRYPT);
	gpg_ctx_set_armor (gpg, TRUE);
	gpg_ctx_set_userid (gpg, userid);
	gpg_ctx_set_istream (gpg, istream);
	gpg_ctx_set_ostream (gpg, ostream);
	gpg_ctx_set_always_trust (gpg, ctx->priv->always_trust);

	for (i = 0; i < recipients->len; i++) {
		gpg_ctx_add_recipient (gpg, recipients->pdata[i]);
	}

	if (!gpg_ctx_op_start (gpg, error))
		goto fail;

	/* FIXME: move this to a common routine */
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

	dw = camel_data_wrapper_new ();
	camel_data_wrapper_construct_from_stream_sync (
		dw, ostream, NULL, NULL);

	encpart = camel_mime_part_new ();
	ct = camel_content_type_new ("application", "octet-stream");
	camel_content_type_set_param (ct, "name", "encrypted.asc");
	camel_data_wrapper_set_mime_type_field (dw, ct);
	camel_content_type_unref (ct);

	camel_medium_set_content ((CamelMedium *) encpart, dw);
	g_object_unref (dw);

	camel_mime_part_set_description (encpart, _("This is a digitally encrypted message part"));

	vstream = camel_stream_mem_new ();
	camel_stream_write_string (vstream, "Version: 1\n", NULL, NULL);
	g_seekable_seek (G_SEEKABLE (vstream), 0, G_SEEK_SET, NULL, NULL);

	verpart = camel_mime_part_new ();
	dw = camel_data_wrapper_new ();
	camel_data_wrapper_set_mime_type (dw, class->encrypt_protocol);
	camel_data_wrapper_construct_from_stream_sync (
		dw, vstream, NULL, NULL);
	g_object_unref (vstream);
	camel_medium_set_content ((CamelMedium *) verpart, dw);
	g_object_unref (dw);

	mpe = camel_multipart_encrypted_new ();
	ct = camel_content_type_new ("multipart", "encrypted");
	camel_content_type_set_param (ct, "protocol", class->encrypt_protocol);
	camel_data_wrapper_set_mime_type_field ((CamelDataWrapper *) mpe, ct);
	camel_content_type_unref (ct);
	camel_multipart_set_boundary ((CamelMultipart *) mpe, NULL);

	mpe->decrypted = g_object_ref (ipart);

	camel_multipart_add_part ((CamelMultipart *) mpe, verpart);
	g_object_unref (verpart);
	camel_multipart_add_part ((CamelMultipart *) mpe, encpart);
	g_object_unref (encpart);

	camel_medium_set_content ((CamelMedium *) opart, (CamelDataWrapper *) mpe);
fail:
	gpg_ctx_free (gpg);
fail1:
	g_object_unref (istream);
	g_object_unref (ostream);

	return success;
}