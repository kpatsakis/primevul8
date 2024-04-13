static void
MYSQLND_METHOD(mysqlnd_protocol, init_prepare_response_packet)(struct st_mysqlnd_packet_prepare_response *packet)
{
	DBG_ENTER("mysqlnd_protocol::init_prepare_response_packet");
	memset(packet, 0, sizeof(*packet));
	packet->header.m = &packet_methods[PROT_PREPARE_RESP_PACKET];
	DBG_VOID_RETURN;