mm_auth_rsa_verify_response(Key *key, BIGNUM *p, u_char response[16])
{
	Buffer m;
	u_char *blob;
	u_int blen;
	int success = 0;

	debug3("%s entering", __func__);

	key->type = KEY_RSA;    /* XXX cheat for key_to_blob */
	if (key_to_blob(key, &blob, &blen) == 0)
		fatal("%s: key_to_blob failed", __func__);
	key->type = KEY_RSA1;

	buffer_init(&m);
	buffer_put_string(&m, blob, blen);
	buffer_put_string(&m, response, 16);
	xfree(blob);

	mm_request_send(pmonitor->m_recvfd, MONITOR_REQ_RSARESPONSE, &m);
	mm_request_receive_expect(pmonitor->m_recvfd, MONITOR_ANS_RSARESPONSE, &m);

	success = buffer_get_int(&m);
	buffer_free(&m);

	return (success);
}