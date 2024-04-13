mm_answer_keyallowed(int socket, Buffer *m)
{
	Key *key;
	char *cuser, *chost;
	u_char *blob;
	u_int bloblen;
	enum mm_keytype type = 0;
	int allowed = 0;

	debug3("%s entering", __func__);

	type = buffer_get_int(m);
	cuser = buffer_get_string(m, NULL);
	chost = buffer_get_string(m, NULL);
	blob = buffer_get_string(m, &bloblen);

	key = key_from_blob(blob, bloblen);

	if ((compat20 && type == MM_RSAHOSTKEY) ||
	    (!compat20 && type != MM_RSAHOSTKEY))
		fatal("%s: key type and protocol mismatch", __func__);

	debug3("%s: key_from_blob: %p", __func__, key);

	if (key != NULL && authctxt->pw != NULL) {
		switch(type) {
		case MM_USERKEY:
			allowed = options.pubkey_authentication &&
			    user_key_allowed(authctxt->pw, key);
			break;
		case MM_HOSTKEY:
			allowed = options.hostbased_authentication &&
			    hostbased_key_allowed(authctxt->pw,
			    cuser, chost, key);
			break;
		case MM_RSAHOSTKEY:
			key->type = KEY_RSA1; /* XXX */
			allowed = options.rhosts_rsa_authentication &&
			    auth_rhosts_rsa_key_allowed(authctxt->pw,
			    cuser, chost, key);
			break;
		default:
			fatal("%s: unknown key type %d", __func__, type);
			break;
		}
	}
	if (key != NULL)
		key_free(key);

	/* clear temporarily storage (used by verify) */
	monitor_reset_key_state();

	if (allowed) {
		/* Save temporarily for comparison in verify */
		key_blob = blob;
		key_bloblen = bloblen;
		key_blobtype = type;
		hostbased_cuser = cuser;
		hostbased_chost = chost;
	}

	debug3("%s: key %p is %s",
	    __func__, key, allowed ? "allowed" : "disallowed");

	buffer_clear(m);
	buffer_put_int(m, allowed);
	buffer_put_int(m, forced_command != NULL);

	mm_append_debug(m);

	mm_request_send(socket, MONITOR_ANS_KEYALLOWED, m);

	if (type == MM_RSAHOSTKEY)
		monitor_permit(mon_dispatch, MONITOR_REQ_RSACHALLENGE, allowed);

	return (0);
}