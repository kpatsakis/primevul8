gpg_decrypt_sync (CamelCipherContext *context,
                  CamelMimePart *ipart,
                  CamelMimePart *opart,
                  GCancellable *cancellable,
                  GError **error)
{
	struct _GpgCtx *gpg = NULL;
	CamelCipherValidity *valid = NULL;
	CamelStream *ostream, *istream;
	CamelDataWrapper *content;
	CamelMimePart *encrypted;
	CamelMultipart *mp;
	CamelContentType *ct;
	gboolean success;

	if (!ipart) {
		g_set_error (
			error, CAMEL_ERROR, CAMEL_ERROR_GENERIC,
			_("Cannot decrypt message: Incorrect message format"));
		return NULL;
	}

	content = camel_medium_get_content ((CamelMedium *) ipart);

	if (!content) {
		g_set_error (
			error, CAMEL_ERROR, CAMEL_ERROR_GENERIC,
			_("Cannot decrypt message: Incorrect message format"));
		return NULL;
	}

	ct = camel_data_wrapper_get_mime_type_field (content);
	/* Encrypted part (using our fake mime type) or PGP/Mime multipart */
	if (camel_content_type_is (ct, "multipart", "encrypted")) {
		mp = (CamelMultipart *) camel_medium_get_content ((CamelMedium *) ipart);
		if (!(encrypted = camel_multipart_get_part (mp, CAMEL_MULTIPART_ENCRYPTED_CONTENT))) {
			g_set_error (
				error, CAMEL_ERROR, CAMEL_ERROR_GENERIC,
				_("Failed to decrypt MIME part: "
				"protocol error"));
			return NULL;
		}

		content = camel_medium_get_content ((CamelMedium *) encrypted);
	} else if (camel_content_type_is (ct, "application", "x-inlinepgp-encrypted")) {
		content = camel_medium_get_content ((CamelMedium *) ipart);
	} else {
		/* Invalid Mimetype */
		g_set_error (
			error, CAMEL_ERROR, CAMEL_ERROR_GENERIC,
			_("Cannot decrypt message: Incorrect message format"));
		return NULL;
	}

	istream = camel_stream_mem_new ();
	if (!camel_data_wrapper_decode_to_stream_sync (
		content, istream, cancellable, error)) {
		g_object_unref (istream);
		return NULL;
	}

	g_seekable_seek (G_SEEKABLE (istream), 0, G_SEEK_SET, NULL, NULL);

	ostream = camel_stream_mem_new ();
	camel_stream_mem_set_secure ((CamelStreamMem *) ostream);

	gpg = gpg_ctx_new (context);
	gpg_ctx_set_mode (gpg, GPG_CTX_MODE_DECRYPT);
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

	/* Report errors only if nothing was decrypted; missing sender's key used
	   for signature of a signed and encrypted messages causes GPG to return
	   failure, thus count with it.
	 */
	if (gpg_ctx_op_wait (gpg) != 0 && gpg->nodata) {
		const gchar *diagnostics;

		diagnostics = gpg_ctx_get_diagnostics (gpg);
		g_set_error (
			error, CAMEL_ERROR, CAMEL_ERROR_GENERIC, "%s",
			(diagnostics != NULL && *diagnostics != '\0') ?
			diagnostics : _("Failed to execute gpg."));
		goto fail;
	}

	g_seekable_seek (G_SEEKABLE (ostream), 0, G_SEEK_SET, NULL, NULL);

	if (camel_content_type_is (ct, "multipart", "encrypted")) {
		CamelDataWrapper *dw;
		CamelStream *null = camel_stream_null_new ();

		/* Multipart encrypted - parse a full mime part */
		success = camel_data_wrapper_construct_from_stream_sync (
			CAMEL_DATA_WRAPPER (opart),
			ostream, NULL, error);

		dw = camel_medium_get_content ((CamelMedium *) opart);
		if (!camel_data_wrapper_decode_to_stream_sync (
			dw, null, cancellable, NULL)) {
			/* nothing had been decoded from the stream, it doesn't
			 * contain any header, like Content-Type or such, thus
			 * write it as a message body */
			success = camel_data_wrapper_construct_from_stream_sync (
				dw, ostream, cancellable, error);
		}

		g_object_unref (null);
	} else {
		/* Inline signed - raw data (may not be a mime part) */
		CamelDataWrapper *dw;
		dw = camel_data_wrapper_new ();
		success = camel_data_wrapper_construct_from_stream_sync (
			dw, ostream, NULL, error);
		camel_data_wrapper_set_mime_type (dw, "application/octet-stream");
		camel_medium_set_content ((CamelMedium *) opart, dw);
		g_object_unref (dw);
		/* Set mime/type of this new part to application/octet-stream to force type snooping */
		camel_mime_part_set_content_type (opart, "application/octet-stream");
	}

	if (success) {
		valid = camel_cipher_validity_new ();
		valid->encrypt.description = g_strdup (_("Encrypted content"));
		valid->encrypt.status = CAMEL_CIPHER_VALIDITY_ENCRYPT_ENCRYPTED;

		if (gpg->hadsig) {
			if (gpg->validsig) {
				if (gpg->trust == GPG_TRUST_UNDEFINED || gpg->trust == GPG_TRUST_NONE)
					valid->sign.status = CAMEL_CIPHER_VALIDITY_SIGN_UNKNOWN;
				else if (gpg->trust != GPG_TRUST_NEVER)
					valid->sign.status = CAMEL_CIPHER_VALIDITY_SIGN_GOOD;
				else
					valid->sign.status = CAMEL_CIPHER_VALIDITY_SIGN_BAD;
			} else if (gpg->nopubkey) {
				valid->sign.status = CAMEL_CIPHER_VALIDITY_SIGN_NEED_PUBLIC_KEY;
			} else {
				valid->sign.status = CAMEL_CIPHER_VALIDITY_SIGN_BAD;
			}

			add_signers (valid, gpg->signers);
		}
	}

 fail:
	g_object_unref (ostream);
	g_object_unref (istream);
	gpg_ctx_free (gpg);

	return valid;
}