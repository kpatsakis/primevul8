mm_answer_rsa_keyallowed(int socket, Buffer *m)
{
	BIGNUM *client_n;
	Key *key = NULL;
	u_char *blob = NULL;
	u_int blen = 0;
	int allowed = 0;

	debug3("%s entering", __func__);

	if (options.rsa_authentication && authctxt->valid) {
		if ((client_n = BN_new()) == NULL)
			fatal("%s: BN_new", __func__);
		buffer_get_bignum2(m, client_n);
		allowed = auth_rsa_key_allowed(authctxt->pw, client_n, &key);
		BN_clear_free(client_n);
	}
	buffer_clear(m);
	buffer_put_int(m, allowed);
	buffer_put_int(m, forced_command != NULL);

	/* clear temporarily storage (used by generate challenge) */
	monitor_reset_key_state();

	if (allowed && key != NULL) {
		key->type = KEY_RSA;	/* cheat for key_to_blob */
		if (key_to_blob(key, &blob, &blen) == 0)
			fatal("%s: key_to_blob failed", __func__);
		buffer_put_string(m, blob, blen);

		/* Save temporarily for comparison in verify */
		key_blob = blob;
		key_bloblen = blen;
		key_blobtype = MM_RSAUSERKEY;
	}
	if (key != NULL)
		key_free(key);

	mm_append_debug(m);

	mm_request_send(socket, MONITOR_ANS_RSAKEYALLOWED, m);

	monitor_permit(mon_dispatch, MONITOR_REQ_RSACHALLENGE, allowed);
	monitor_permit(mon_dispatch, MONITOR_REQ_RSARESPONSE, 0);
	return (0);
}