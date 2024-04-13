mm_answer_rsa_challenge(int socket, Buffer *m)
{
	Key *key = NULL;
	u_char *blob;
	u_int blen;

	debug3("%s entering", __func__);

	if (!authctxt->valid)
		fatal("%s: authctxt not valid", __func__);
	blob = buffer_get_string(m, &blen);
	if (!monitor_allowed_key(blob, blen))
		fatal("%s: bad key, not previously allowed", __func__);
	if (key_blobtype != MM_RSAUSERKEY && key_blobtype != MM_RSAHOSTKEY)
		fatal("%s: key type mismatch", __func__);
	if ((key = key_from_blob(blob, blen)) == NULL)
		fatal("%s: received bad key", __func__);

	if (ssh1_challenge)
		BN_clear_free(ssh1_challenge);
	ssh1_challenge = auth_rsa_generate_challenge(key);

	buffer_clear(m);
	buffer_put_bignum2(m, ssh1_challenge);

	debug3("%s sending reply", __func__);
	mm_request_send(socket, MONITOR_ANS_RSACHALLENGE, m);

	monitor_permit(mon_dispatch, MONITOR_REQ_RSARESPONSE, 1);

	xfree(blob);
	key_free(key);
	return (0);
}