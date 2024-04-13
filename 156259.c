monitor_valid_userblob(u_char *data, u_int datalen)
{
	Buffer b;
	char *p;
	u_int len;
	int fail = 0;

	buffer_init(&b);
	buffer_append(&b, data, datalen);

	if (datafellows & SSH_OLD_SESSIONID) {
		p = buffer_ptr(&b);
		len = buffer_len(&b);
		if ((session_id2 == NULL) ||
		    (len < session_id2_len) ||
		    (memcmp(p, session_id2, session_id2_len) != 0))
			fail++;
		buffer_consume(&b, session_id2_len);
	} else {
		p = buffer_get_string(&b, &len);
		if ((session_id2 == NULL) ||
		    (len != session_id2_len) ||
		    (memcmp(p, session_id2, session_id2_len) != 0))
			fail++;
		xfree(p);
	}
	if (buffer_get_char(&b) != SSH2_MSG_USERAUTH_REQUEST)
		fail++;
	p = buffer_get_string(&b, NULL);
	if (strcmp(authctxt->user, p) != 0) {
		logit("wrong user name passed to monitor: expected %s != %.100s",
		    authctxt->user, p);
		fail++;
	}
	xfree(p);
	buffer_skip_string(&b);
	if (datafellows & SSH_BUG_PKAUTH) {
		if (!buffer_get_char(&b))
			fail++;
	} else {
		p = buffer_get_string(&b, NULL);
		if (strcmp("publickey", p) != 0)
			fail++;
		xfree(p);
		if (!buffer_get_char(&b))
			fail++;
		buffer_skip_string(&b);
	}
	buffer_skip_string(&b);
	if (buffer_len(&b) != 0)
		fail++;
	buffer_free(&b);
	return (fail == 0);
}