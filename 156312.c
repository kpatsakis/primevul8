mm_answer_sign(int socket, Buffer *m)
{
	Key *key;
	u_char *p;
	u_char *signature;
	u_int siglen, datlen;
	int keyid;

	debug3("%s", __func__);

	keyid = buffer_get_int(m);
	p = buffer_get_string(m, &datlen);

	if (datlen != 20)
		fatal("%s: data length incorrect: %u", __func__, datlen);

	/* save session id, it will be passed on the first call */
	if (session_id2_len == 0) {
		session_id2_len = datlen;
		session_id2 = xmalloc(session_id2_len);
		memcpy(session_id2, p, session_id2_len);
	}

	if ((key = get_hostkey_by_index(keyid)) == NULL)
		fatal("%s: no hostkey from index %d", __func__, keyid);
	if (key_sign(key, &signature, &siglen, p, datlen) < 0)
		fatal("%s: key_sign failed", __func__);

	debug3("%s: signature %p(%u)", __func__, signature, siglen);

	buffer_clear(m);
	buffer_put_string(m, signature, siglen);

	xfree(p);
	xfree(signature);

	mm_request_send(socket, MONITOR_ANS_SIGN, m);

	/* Turn on permissions for getpwnam */
	monitor_permit(mon_dispatch, MONITOR_REQ_PWNAM, 1);

	return (0);
}