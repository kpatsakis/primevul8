static void
MYSQLND_METHOD(mysqlnd_protocol, init_eof_packet)(struct st_mysqlnd_packet_eof *packet)
{
	DBG_ENTER("mysqlnd_protocol::init_eof_packet");
	memset(packet, 0, sizeof(*packet));
	packet->header.m = &packet_methods[PROT_EOF_PACKET];
	DBG_VOID_RETURN;