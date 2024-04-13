static void
MYSQLND_METHOD(mysqlnd_protocol, init_sha256_pk_request_packet)(struct st_mysqlnd_packet_sha256_pk_request *packet)
{
	DBG_ENTER("mysqlnd_protocol::init_sha256_pk_request_packet");
	memset(packet, 0, sizeof(*packet));
	packet->header.m = &packet_methods[PROT_SHA256_PK_REQUEST_PACKET];
	DBG_VOID_RETURN;