mm_auth_krb5(void *ctx, void *argp, char **userp, void *resp)
{
	krb5_data *tkt, *reply;
	Buffer m;
	int success;

	debug3("%s entering", __func__);
	tkt = (krb5_data *) argp;
	reply = (krb5_data *) resp;

	buffer_init(&m);
	buffer_put_string(&m, tkt->data, tkt->length);

	mm_request_send(pmonitor->m_recvfd, MONITOR_REQ_KRB5, &m);
	mm_request_receive_expect(pmonitor->m_recvfd, MONITOR_ANS_KRB5, &m);

	success = buffer_get_int(&m);
	if (success) {
		u_int len;

		*userp = buffer_get_string(&m, NULL);
		reply->data = buffer_get_string(&m, &len);
		reply->length = len;
	} else {
		memset(reply, 0, sizeof(*reply));
		*userp = NULL;
	}

	buffer_free(&m);
	return (success);
}