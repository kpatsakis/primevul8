static void
MYSQLND_METHOD(mysqlnd_protocol, init_cached_sha2_result_packet)(struct st_mysqlnd_packet_cached_sha2_result *packet)
{
	DBG_ENTER("mysqlnd_protocol::init_cached_sha2_result_packet");
	memset(packet, 0, sizeof(*packet));
	packet->header.m = &packet_methods[PROT_CACHED_SHA2_RESULT_PACKET];
	DBG_VOID_RETURN;