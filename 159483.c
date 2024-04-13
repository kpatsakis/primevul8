static void
MYSQLND_METHOD(mysqlnd_protocol, init_greet_packet)(struct st_mysqlnd_packet_greet *packet)
{
	DBG_ENTER("mysqlnd_protocol::init_greet_packet");
	memset(packet, 0, sizeof(*packet));
	packet->header.m = &packet_methods[PROT_GREET_PACKET];
	DBG_VOID_RETURN;