static void
MYSQLND_METHOD(mysqlnd_protocol, init_ok_packet)(struct st_mysqlnd_packet_ok *packet)
{
	DBG_ENTER("mysqlnd_protocol::init_ok_packet");
	memset(packet, 0, sizeof(*packet));
	packet->header.m = &packet_methods[PROT_OK_PACKET];
	DBG_VOID_RETURN;