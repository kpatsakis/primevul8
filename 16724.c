gpg_verify_sync (CamelCipherContext *context,
                 CamelMimePart *ipart,
                 GCancellable *cancellable,
                 GError **error)
{
	CamelCipherContextClass *class;
	CamelCipherValidity *validity;
	const gchar *diagnostics = NULL;
	struct _GpgCtx *gpg = NULL;
	gchar *sigfile = NULL;
	CamelContentType *ct;
	CamelMimePart *sigpart;
	CamelStream *istream = NULL, *canon_stream;
	CamelMultipart *mps;
	CamelStream *filter;
	CamelMimeFilter *canon;

	class = CAMEL_CIPHER_CONTEXT_GET_CLASS (context);

	mps = (CamelMultipart *) camel_medium_get_content ((CamelMedium *) ipart);
	ct = ((CamelDataWrapper *) mps)->mime_type;

	/* Inline signature (using our fake mime type) or PGP/Mime signature */
	if (camel_content_type_is (ct, "multipart", "signed")) {
		/* PGP/Mime Signature */
		const gchar *tmp;

		tmp = camel_content_type_param (ct, "protocol");
		if (!CAMEL_IS_MULTIPART_SIGNED (mps)
		    || tmp == NULL
		    || g_ascii_strcasecmp (tmp, class->sign_protocol) != 0) {
			g_set_error (
				error, CAMEL_ERROR, CAMEL_ERROR_GENERIC,
				_("Cannot verify message signature: "
				"Incorrect message format"));
			return NULL;
		}

		if (!(istream = camel_multipart_signed_get_content_stream ((CamelMultipartSigned *) mps, NULL))) {
			g_set_error (
				error, CAMEL_ERROR, CAMEL_ERROR_GENERIC,
				_("Cannot verify message signature: "
				"Incorrect message format"));
			return NULL;
		}

		if (!(sigpart = camel_multipart_get_part (mps, CAMEL_MULTIPART_SIGNED_SIGNATURE))) {
			g_set_error (
				error, CAMEL_ERROR, CAMEL_ERROR_GENERIC,
				_("Cannot verify message signature: "
				"Incorrect message format"));
			g_object_unref (istream);
			return NULL;
		}
	} else if (camel_content_type_is (ct, "application", "x-inlinepgp-signed")) {
		/* Inline Signed */
		CamelDataWrapper *content;
		content = camel_medium_get_content ((CamelMedium *) ipart);
		istream = camel_stream_mem_new ();
		if (!camel_data_wrapper_decode_to_stream_sync (
			content, istream, cancellable, error))
			goto exception;
		g_seekable_seek (
			G_SEEKABLE (istream), 0, G_SEEK_SET, NULL, NULL);
		sigpart = NULL;
	} else {
		/* Invalid Mimetype */
		g_set_error (
			error, CAMEL_ERROR, CAMEL_ERROR_GENERIC,
			_("Cannot verify message signature: "
			"Incorrect message format"));
		return NULL;
	}

	/* Now start the real work of verifying the message */
#ifdef GPG_LOG
	if (camel_debug_start ("gpg:sign")) {
		gchar *name;
		CamelStream *out;

		name = g_strdup_printf ("camel-gpg.%d.verify.data", logid);
		out = camel_stream_fs_new_with_name (name, O_CREAT | O_TRUNC | O_WRONLY, 0666);
		if (out) {
			printf ("Writing gpg verify data to '%s'\n", name);
			camel_stream_write_to_stream (istream, out);
			g_seekable_seek (
				G_SEEKABLE (istream),
				0, G_SEEK_SET, NULL, NULL);
			g_object_unref (out);
		}

		g_free (name);

		if (sigpart) {
			name = g_strdup_printf ("camel-gpg.%d.verify.signature", logid++);
			out = camel_stream_fs_new_with_name (name, O_CREAT | O_TRUNC | O_WRONLY, 0666);
			if (out) {
				printf ("Writing gpg verify signature to '%s'\n", name);
				camel_data_wrapper_write_to_stream ((CamelDataWrapper *) sigpart, out);
				g_object_unref (out);
			}
			g_free (name);
		}
		camel_debug_end ();
	}
#endif

	if (sigpart) {
		sigfile = swrite (sigpart, cancellable, error);
		if (sigfile == NULL) {
			g_prefix_error (
				error, _("Cannot verify message signature: "));
			goto exception;
		}
	}

	g_seekable_seek (G_SEEKABLE (istream), 0, G_SEEK_SET, NULL, NULL);

	canon_stream = camel_stream_mem_new ();

	/* strip trailing white-spaces */
	filter = camel_stream_filter_new (canon_stream);
	canon = camel_mime_filter_canon_new (CAMEL_MIME_FILTER_CANON_CRLF | CAMEL_MIME_FILTER_CANON_STRIP);
	camel_stream_filter_add (CAMEL_STREAM_FILTER (filter), canon);
	g_object_unref (canon);

	camel_stream_write_to_stream (istream, filter, NULL, NULL);

	g_object_unref (filter);

	g_seekable_seek (G_SEEKABLE (istream), 0, G_SEEK_SET, NULL, NULL);

	g_seekable_seek (G_SEEKABLE (canon_stream), 0, G_SEEK_SET, NULL, NULL);

	gpg = gpg_ctx_new (context);
	gpg_ctx_set_mode (gpg, GPG_CTX_MODE_VERIFY);
	if (sigfile)
		gpg_ctx_set_sigfile (gpg, sigfile);
	gpg_ctx_set_istream (gpg, canon_stream);

	if (!gpg_ctx_op_start (gpg, error))
		goto exception;

	while (!gpg_ctx_op_complete (gpg)) {
		if (gpg_ctx_op_step (gpg, cancellable, error) == -1) {
			gpg_ctx_op_cancel (gpg);
			goto exception;
		}
	}

	/* report error only when no data or didn't found signature */
	if (gpg_ctx_op_wait (gpg) != 0 && (gpg->nodata || !gpg->hadsig)) {
		const gchar *diagnostics;

		diagnostics = gpg_ctx_get_diagnostics (gpg);
		g_set_error (
			error, CAMEL_ERROR, CAMEL_ERROR_GENERIC, "%s",
			(diagnostics != NULL && *diagnostics != '\0') ?
			diagnostics : _("Failed to execute gpg."));
		goto exception;
	}

	validity = camel_cipher_validity_new ();
	diagnostics = gpg_ctx_get_diagnostics (gpg);
	camel_cipher_validity_set_description (validity, diagnostics);
	if (gpg->validsig) {
		if (gpg->trust == GPG_TRUST_UNDEFINED || gpg->trust == GPG_TRUST_NONE)
			validity->sign.status = CAMEL_CIPHER_VALIDITY_SIGN_UNKNOWN;
		else if (gpg->trust != GPG_TRUST_NEVER)
			validity->sign.status = CAMEL_CIPHER_VALIDITY_SIGN_GOOD;
		else
			validity->sign.status = CAMEL_CIPHER_VALIDITY_SIGN_BAD;
	} else if (gpg->nopubkey) {
		validity->sign.status = CAMEL_CIPHER_VALIDITY_SIGN_NEED_PUBLIC_KEY;
	} else {
		validity->sign.status = CAMEL_CIPHER_VALIDITY_SIGN_BAD;
	}

	add_signers (validity, gpg->signers);

	gpg_ctx_free (gpg);

	if (sigfile) {
		g_unlink (sigfile);
		g_free (sigfile);
	}
	g_object_unref (istream);
	g_object_unref (canon_stream);

	return validity;

 exception:

	if (gpg != NULL)
		gpg_ctx_free (gpg);

	if (istream)
		g_object_unref (istream);

	if (sigfile) {
		g_unlink (sigfile);
		g_free (sigfile);
	}

	return NULL;
}