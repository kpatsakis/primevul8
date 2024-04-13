static void
MYSQLND_METHOD(mysqlnd_protocol, init_auth_response_packet)(struct st_mysqlnd_packet_auth_response *packet)
{
	DBG_ENTER("mysqlnd_protocol::init_auth_response_packet");
	memset(packet, 0, sizeof(*packet));
	packet->header.m = &packet_methods[PROT_AUTH_RESP_PACKET];
	DBG_VOID_RETURN;