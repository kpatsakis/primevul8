static void
php_mysqlnd_sha256_pk_request_response_free_mem(void * _packet)
{
	MYSQLND_PACKET_SHA256_PK_REQUEST_RESPONSE * p = (MYSQLND_PACKET_SHA256_PK_REQUEST_RESPONSE *) _packet;
	if (p->public_key) {
		mnd_efree(p->public_key);
		p->public_key = NULL;
	}
	p->public_key_len = 0;