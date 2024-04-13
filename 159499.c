static void
MYSQLND_METHOD(mysqlnd_protocol, init_result_field_packet)(struct st_mysqlnd_packet_res_field *packet)
{
	DBG_ENTER("mysqlnd_protocol::init_result_field_packet");
	memset(packet, 0, sizeof(*packet));
	packet->header.m = &packet_methods[PROT_RSET_FLD_PACKET];
	DBG_VOID_RETURN;