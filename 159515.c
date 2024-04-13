static void
MYSQLND_METHOD(mysqlnd_protocol, init_stats_packet)(struct st_mysqlnd_packet_stats *packet)
{
	DBG_ENTER("mysqlnd_protocol::init_stats_packet");
	memset(packet, 0, sizeof(*packet));
	packet->header.m = &packet_methods[PROT_STATS_PACKET];
	DBG_VOID_RETURN;