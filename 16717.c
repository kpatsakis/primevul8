gpg_sign_sync (CamelCipherContext *context,
               const gchar *userid,
               CamelCipherHash hash,
               CamelMimePart *ipart,
               CamelMimePart *opart,
               GCancellable *cancellable,
               GError **error)
{
	struct _GpgCtx *gpg = NULL;
	CamelCipherContextClass *class;
	CamelStream *ostream = camel_stream_mem_new (), *istream;
	CamelDataWrapper *dw;
	CamelContentType *ct;
	CamelMimePart *sigpart;
	CamelMultipartSigned *mps;
	gboolean success = FALSE;

	/* Note: see rfc2015 or rfc3156, section 5 */

	class = CAMEL_CIPHER_CONTEXT_GET_CLASS (context);

	/* FIXME: stream this, we stream output at least */
	istream = camel_stream_mem_new ();
	if (camel_cipher_canonical_to_stream (
		ipart, CAMEL_MIME_FILTER_CANON_STRIP |
		CAMEL_MIME_FILTER_CANON_CRLF |
		CAMEL_MIME_FILTER_CANON_FROM,
		istream, NULL, error) == -1) {
		g_prefix_error (
			error, _("Could not generate signing data: "));
		goto fail;
	}

#ifdef GPG_LOG
	if (camel_debug_start ("gpg:sign")) {
		gchar *name;
		CamelStream *out;

		name = g_strdup_printf ("camel-gpg.%d.sign-data", logid++);
		out = camel_stream_fs_new_with_name (name, O_CREAT | O_TRUNC | O_WRONLY, 0666);
		if (out) {
			printf ("Writing gpg signing data to '%s'\n", name);
			camel_stream_write_to_stream (istream, out);
			g_seekable_seek (
				G_SEEKABLE (istream), 0,
				G_SEEK_SET, NULL, NULL);
			g_object_unref (out);
		}
		g_free (name);
		camel_debug_end ();
	}
#endif

	gpg = gpg_ctx_new (context);
	gpg_ctx_set_mode (gpg, GPG_CTX_MODE_SIGN);
	gpg_ctx_set_hash (gpg, hash);
	gpg_ctx_set_armor (gpg, TRUE);
	gpg_ctx_set_userid (gpg, userid);
	gpg_ctx_set_istream (gpg, istream);
	gpg_ctx_set_ostream (gpg, ostream);

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

	dw = camel_data_wrapper_new ();
	g_seekable_seek (G_SEEKABLE (ostream), 0, G_SEEK_SET, NULL, NULL);
	camel_data_wrapper_construct_from_stream_sync (
		dw, ostream, NULL, NULL);

	sigpart = camel_mime_part_new ();
	ct = camel_content_type_new ("application", "pgp-signature");
	camel_content_type_set_param (ct, "name", "signature.asc");
	camel_data_wrapper_set_mime_type_field (dw, ct);
	camel_content_type_unref (ct);

	camel_medium_set_content ((CamelMedium *) sigpart, dw);
	g_object_unref (dw);

	camel_mime_part_set_description (sigpart, "This is a digitally signed message part");

	mps = camel_multipart_signed_new ();
	ct = camel_content_type_new ("multipart", "signed");
	camel_content_type_set_param (ct, "micalg", camel_cipher_context_hash_to_id (context, hash == CAMEL_CIPHER_HASH_DEFAULT ? gpg->hash : hash));
	camel_content_type_set_param (ct, "protocol", class->sign_protocol);
	camel_data_wrapper_set_mime_type_field ((CamelDataWrapper *) mps, ct);
	camel_content_type_unref (ct);
	camel_multipart_set_boundary ((CamelMultipart *) mps, NULL);

	mps->signature = sigpart;
	mps->contentraw = g_object_ref (istream);
	g_seekable_seek (G_SEEKABLE (istream), 0, G_SEEK_SET, NULL, NULL);

	camel_medium_set_content ((CamelMedium *) opart, (CamelDataWrapper *) mps);
fail:
	g_object_unref (ostream);

	if (gpg)
		gpg_ctx_free (gpg);

	return success;
}