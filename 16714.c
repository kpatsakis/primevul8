gpg_ctx_parse_status (struct _GpgCtx *gpg,
                      GError **error)
{
	register guchar *inptr;
	const guchar *status;
	gsize nread, nwritten;
	gint len;

 parse:

	inptr = gpg->statusbuf;
	while (inptr < gpg->statusptr && *inptr != '\n')
		inptr++;

	if (*inptr != '\n') {
		/* we don't have enough data buffered to parse this status line */
		return 0;
	}

	*inptr++ = '\0';
	status = gpg->statusbuf;

	if (camel_debug ("gpg:status"))
		printf ("status: %s\n", status);

	if (strncmp ((const gchar *) status, "[GNUPG:] ", 9) != 0) {
		gchar *message;

		message = g_locale_to_utf8 (
			(const gchar *) status, -1, NULL, NULL, NULL);
		g_set_error (
			error, CAMEL_ERROR, CAMEL_ERROR_GENERIC,
			_("Unexpected GnuPG status message encountered:\n\n%s"),
			message);
		g_free (message);

		return -1;
	}

	status += 9;

	if (!strncmp ((gchar *) status, "ENC_TO ", 7)) {
		gchar *key = NULL;

		status += 7;

		status = (const guchar *) next_token ((gchar *) status, &key);
		if (key) {
			gboolean all_zero = *key == '0';
			gint i = 0;

			while (key[i] && all_zero) {
				all_zero = key[i] == '0';
				i++;
			}

			gpg->anonymous_recipient = all_zero;

			g_free (key);
		}
	} else if (!strncmp ((gchar *) status, "USERID_HINT ", 12)) {
		gchar *hint, *user;

		status += 12;
		status = (const guchar *) next_token ((gchar *) status, &hint);
		if (!hint) {
			g_set_error (
				error, CAMEL_ERROR, CAMEL_ERROR_GENERIC,
				_("Failed to parse gpg userid hint."));
			return -1;
		}

		if (g_hash_table_lookup (gpg->userid_hint, hint)) {
			/* we already have this userid hint... */
			g_free (hint);
			goto recycle;
		}

		if (gpg->utf8 || !(user = g_locale_to_utf8 ((gchar *) status, -1, &nread, &nwritten, NULL)))
			user = g_strdup ((gchar *) status);

		g_strstrip (user);

		g_hash_table_insert (gpg->userid_hint, hint, user);
	} else if (!strncmp ((gchar *) status, "NEED_PASSPHRASE ", 16)) {
		gchar *userid;

		status += 16;

		status = (const guchar *) next_token ((gchar *) status, &userid);
		if (!userid) {
			g_set_error (
				error, CAMEL_ERROR, CAMEL_ERROR_GENERIC,
				_("Failed to parse gpg passphrase request."));
			return -1;
		}

		g_free (gpg->need_id);
		gpg->need_id = userid;
	} else if (!strncmp ((gchar *) status, "NEED_PASSPHRASE_PIN ", 20)) {
		gchar *userid;

		status += 20;

		status = (const guchar *) next_token ((gchar *) status, &userid);
		if (!userid) {
			g_set_error (
				error, CAMEL_ERROR, CAMEL_ERROR_GENERIC,
				_("Failed to parse gpg passphrase request."));
			return -1;
		}

		g_free (gpg->need_id);
		gpg->need_id = userid;
	} else if (!strncmp ((gchar *) status, "GET_HIDDEN ", 11)) {
		const gchar *name = NULL;
		gchar *prompt, *passwd;
		guint32 flags;
		GError *local_error = NULL;

		status += 11;

		if (gpg->need_id && !(name = g_hash_table_lookup (gpg->userid_hint, gpg->need_id)))
			name = gpg->need_id;
		else if (!name)
			name = "";

		if (!strncmp ((gchar *) status, "passphrase.pin.ask", 18)) {
			prompt = g_markup_printf_escaped (
				_("You need a PIN to unlock the key for your\n"
				"SmartCard: \"%s\""), name);
		} else if (!strncmp ((gchar *) status, "passphrase.enter", 16)) {
			prompt = g_markup_printf_escaped (
				_("You need a passphrase to unlock the key for\n"
				"user: \"%s\""), name);
		} else {
			next_token ((gchar *) status, &prompt);
			g_set_error (
				error, CAMEL_ERROR, CAMEL_ERROR_GENERIC,
				_("Unexpected request from GnuPG for '%s'"),
				prompt);
			g_free (prompt);
			return -1;
		}

		if (gpg->anonymous_recipient) {
			gchar *tmp = prompt;

			/* FIXME Reword prompt message. */
			prompt = g_strconcat (
				tmp, "\n",
				_("Note the encrypted content doesn't contain "
				"information about a recipient, thus there "
				"will be a password prompt for each of stored "
				"private key."), NULL);

			g_free (tmp);
		}

		flags = CAMEL_SESSION_PASSWORD_SECRET | CAMEL_SESSION_PASSPHRASE;
		if ((passwd = camel_session_get_password (gpg->session, NULL, prompt, gpg->need_id, flags, &local_error))) {
			if (!gpg->utf8) {
				gchar *opasswd = passwd;

				if ((passwd = g_locale_to_utf8 (passwd, -1, &nread, &nwritten, NULL))) {
					memset (opasswd, 0, strlen (opasswd));
					g_free (opasswd);
				} else {
					passwd = opasswd;
				}
			}

			gpg->passwd = g_strdup_printf ("%s\n", passwd);
			memset (passwd, 0, strlen (passwd));
			g_free (passwd);

			gpg->send_passwd = TRUE;
		} else {
			if (local_error == NULL)
				g_set_error (
					error, G_IO_ERROR,
					G_IO_ERROR_CANCELLED,
					_("Cancelled"));
			g_propagate_error (error, local_error);

			return -1;
		}

		g_free (prompt);
	} else if (!strncmp ((gchar *) status, "GOOD_PASSPHRASE", 15)) {
		gpg->bad_passwds = 0;
	} else if (!strncmp ((gchar *) status, "BAD_PASSPHRASE", 14)) {
		/* with anonymous recipient is user asked for his/her password for each stored key,
		 * thus here cannot be counted wrong passwords */
		if (!gpg->anonymous_recipient) {
			gpg->bad_passwds++;

			camel_session_forget_password (gpg->session, NULL, gpg->need_id, error);

			if (gpg->bad_passwds == 3) {
				g_set_error (
					error, CAMEL_SERVICE_ERROR,
					CAMEL_SERVICE_ERROR_CANT_AUTHENTICATE,
					_("Failed to unlock secret key: "
					"3 bad passphrases given."));
				return -1;
			}
		}
	} else if (!strncmp ((const gchar *) status, "UNEXPECTED ", 11)) {
		/* this is an error */
		gchar *message;

		message = g_locale_to_utf8 (
			(const gchar *) status + 11, -1, NULL, NULL, NULL);
		g_set_error (
			error, CAMEL_ERROR, CAMEL_ERROR_GENERIC,
			_("Unexpected response from GnuPG: %s"), message);
		g_free (message);

		return -1;
	} else if (!strncmp ((gchar *) status, "NODATA", 6)) {
		/* this is an error */
		/* But we ignore it anyway, we should get other response codes to say why */
		gpg->nodata = TRUE;
	} else {
		if (!strncmp ((gchar *) status, "BEGIN_", 6)) {
			gpg->processing = TRUE;
		} else if (!strncmp ((gchar *) status, "END_", 4)) {
			gpg->processing = FALSE;
		}

		/* check to see if we are complete */
		switch (gpg->mode) {
		case GPG_CTX_MODE_SIGN:
			if (!strncmp ((gchar *) status, "SIG_CREATED ", 12)) {
				/* SIG_CREATED <type> <pubkey algo> <hash algo> <class> <timestamp> <key fpr> */
				const gchar *str, *p;
				gint i = 0;

				str = (const gchar *) status + 12;
				while (p = strchr (str, ' '), i < 2 && p) {
					str = p + 1;
					i++;
				}

				if (*str && i == 2) {
					struct {
						gint gpg_hash_algo;
						CamelCipherHash camel_hash_algo;
					} hash_algos[] = {
						/* the rest are deprecated/not supported by gpg any more */
						{  2, CAMEL_CIPHER_HASH_SHA1 },
						{  3, CAMEL_CIPHER_HASH_RIPEMD160 },
						{  8, CAMEL_CIPHER_HASH_SHA256 },
						{  9, CAMEL_CIPHER_HASH_SHA384 },
						{ 10, CAMEL_CIPHER_HASH_SHA512 }
					};

					gint gpg_hash = strtoul (str, NULL, 10);

					for (i = 0; i < G_N_ELEMENTS (hash_algos); i++) {
						if (hash_algos[i].gpg_hash_algo == gpg_hash) {
							gpg->hash = hash_algos[i].camel_hash_algo;
							break;
						}
					}
				}
			}
			break;
		case GPG_CTX_MODE_DECRYPT:
			if (!strncmp ((gchar *) status, "BEGIN_DECRYPTION", 16)) {
				/* nothing to do... but we know to expect data on stdout soon */
				break;
			} else if (!strncmp ((gchar *) status, "END_DECRYPTION", 14)) {
				/* nothing to do, but we know the end is near? */
				break;
			}
			/* let if fall through to verify possible signatures too */
			/* break; */
		case GPG_CTX_MODE_VERIFY:
			if (!strncmp ((gchar *) status, "TRUST_", 6)) {
				status += 6;
				if (!strncmp ((gchar *) status, "NEVER", 5)) {
					gpg->trust = GPG_TRUST_NEVER;
				} else if (!strncmp ((gchar *) status, "MARGINAL", 8)) {
					gpg->trust = GPG_TRUST_MARGINAL;
				} else if (!strncmp ((gchar *) status, "FULLY", 5)) {
					gpg->trust = GPG_TRUST_FULLY;
				} else if (!strncmp ((gchar *) status, "ULTIMATE", 8)) {
					gpg->trust = GPG_TRUST_ULTIMATE;
				} else if (!strncmp ((gchar *) status, "UNDEFINED", 9)) {
					gpg->trust = GPG_TRUST_UNDEFINED;
				}
			} else if (!strncmp ((gchar *) status, "GOODSIG ", 8)) {
				gpg->goodsig = TRUE;
				gpg->hadsig = TRUE;
				status += 8;
				/* there's a key ID, then the email address */
				status = (const guchar *) strchr ((const gchar *) status, ' ');
				if (status) {
					const gchar *str = (const gchar *) status + 1;
					const gchar *eml = strchr (str, '<');

					if (eml && eml > str) {
						eml--;
						if (strchr (str, ' ') >= eml)
							eml = NULL;
					} else {
						eml = NULL;
					}

					if (gpg->signers) {
						g_string_append (gpg->signers, ", ");
					} else {
						gpg->signers = g_string_new ("");
					}

					if (eml) {
						g_string_append (gpg->signers, "\"");
						g_string_append_len (gpg->signers, str, eml - str);
						g_string_append (gpg->signers, "\"");
						g_string_append (gpg->signers, eml);
					} else {
						g_string_append (gpg->signers, str);
					}
				}
			} else if (!strncmp ((gchar *) status, "VALIDSIG ", 9)) {
				gpg->validsig = TRUE;
			} else if (!strncmp ((gchar *) status, "BADSIG ", 7)) {
				gpg->badsig = FALSE;
				gpg->hadsig = TRUE;
			} else if (!strncmp ((gchar *) status, "ERRSIG ", 7)) {
				/* Note: NO_PUBKEY often comes after an ERRSIG */
				gpg->errsig = FALSE;
				gpg->hadsig = TRUE;
			} else if (!strncmp ((gchar *) status, "NO_PUBKEY ", 10)) {
				gpg->nopubkey = TRUE;
			}
			break;
		case GPG_CTX_MODE_ENCRYPT:
			if (!strncmp ((gchar *) status, "BEGIN_ENCRYPTION", 16)) {
				/* nothing to do... but we know to expect data on stdout soon */
			} else if (!strncmp ((gchar *) status, "END_ENCRYPTION", 14)) {
				/* nothing to do, but we know the end is near? */
			} else if (!strncmp ((gchar *) status, "NO_RECP", 7)) {
				g_set_error (
					error, CAMEL_ERROR, CAMEL_ERROR_GENERIC,
					_("Failed to encrypt: No valid recipients specified."));
				return -1;
			}
			break;
		case GPG_CTX_MODE_IMPORT:
			/* noop */
			break;
		case GPG_CTX_MODE_EXPORT:
			/* noop */
			break;
		}
	}

 recycle:

	/* recycle our statusbuf by moving inptr to the beginning of statusbuf */
	len = gpg->statusptr - inptr;
	memmove (gpg->statusbuf, inptr, len);

	len = inptr - gpg->statusbuf;
	gpg->statusleft += len;
	gpg->statusptr -= len;

	/* if we have more data, try parsing the next line? */
	if (gpg->statusptr > gpg->statusbuf)
		goto parse;

	return 0;
}