static void
MYSQLND_METHOD(mysqlnd_protocol, init_row_packet)(struct st_mysqlnd_packet_row *packet)
{
	DBG_ENTER("mysqlnd_protocol::init_row_packet");
	memset(packet, 0, sizeof(*packet));
	packet->header.m = &packet_methods[PROT_ROW_PACKET];
	DBG_VOID_RETURN;