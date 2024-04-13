gpg_ctx_new (CamelCipherContext *context)
{
	struct _GpgCtx *gpg;
	const gchar *charset;
	CamelStream *stream;
	CamelSession *session;

	session = camel_cipher_context_get_session (context);

	gpg = g_new (struct _GpgCtx, 1);
	gpg->mode = GPG_CTX_MODE_SIGN;
	gpg->session = g_object_ref (session);
	gpg->userid_hint = g_hash_table_new (g_str_hash, g_str_equal);
	gpg->complete = FALSE;
	gpg->seen_eof1 = TRUE;
	gpg->seen_eof2 = FALSE;
	gpg->pid = (pid_t) -1;
	gpg->exit_status = 0;
	gpg->exited = FALSE;

	gpg->userids = NULL;
	gpg->sigfile = NULL;
	gpg->recipients = NULL;
	gpg->hash = CAMEL_CIPHER_HASH_DEFAULT;
	gpg->always_trust = FALSE;
	gpg->armor = FALSE;

	gpg->stdin_fd = -1;
	gpg->stdout_fd = -1;
	gpg->stderr_fd = -1;
	gpg->status_fd = -1;
	gpg->passwd_fd = -1;

	gpg->statusbuf = g_malloc (128);
	gpg->statusptr = gpg->statusbuf;
	gpg->statusleft = 128;

	gpg->bad_passwds = 0;
	gpg->anonymous_recipient = FALSE;
	gpg->need_passwd = FALSE;
	gpg->send_passwd = FALSE;
	gpg->need_id = NULL;
	gpg->passwd = NULL;

	gpg->nodata = FALSE;
	gpg->hadsig = FALSE;
	gpg->badsig = FALSE;
	gpg->errsig = FALSE;
	gpg->goodsig = FALSE;
	gpg->validsig = FALSE;
	gpg->nopubkey = FALSE;
	gpg->trust = GPG_TRUST_NONE;
	gpg->processing = FALSE;
	gpg->signers = NULL;

	gpg->istream = NULL;
	gpg->ostream = NULL;

	gpg->diagbuf = g_byte_array_new ();
	gpg->diagflushed = FALSE;

	stream = camel_stream_mem_new_with_byte_array (gpg->diagbuf);

	if ((charset = camel_iconv_locale_charset ()) && g_ascii_strcasecmp (charset, "UTF-8") != 0) {
		CamelMimeFilter *filter;
		CamelStream *fstream;

		gpg->utf8 = FALSE;

		if ((filter = camel_mime_filter_charset_new (charset, "UTF-8"))) {
			fstream = camel_stream_filter_new (stream);
			camel_stream_filter_add (
				CAMEL_STREAM_FILTER (fstream), filter);
			g_object_unref (filter);
			g_object_unref (stream);

			stream = (CamelStream *) fstream;
		}
	} else {
		gpg->utf8 = TRUE;
	}

	gpg->diagnostics = stream;

	return gpg;
}