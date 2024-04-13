static void
MYSQLND_METHOD(mysqlnd_protocol, init_rset_header_packet)(struct st_mysqlnd_packet_rset_header *packet)
{
	DBG_ENTER("mysqlnd_protocol::get_rset_header_packet");
	memset(packet, 0, sizeof(*packet));
	packet->header.m = &packet_methods[PROT_RSET_HEADER_PACKET];
	DBG_VOID_RETURN;