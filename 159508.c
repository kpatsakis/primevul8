static void
MYSQLND_METHOD(mysqlnd_protocol, init_change_user_response_packet)(struct st_mysqlnd_packet_chg_user_resp *packet)
{
	DBG_ENTER("mysqlnd_protocol::init_change_user_response_packet");
	memset(packet, 0, sizeof(*packet));
	packet->header.m = &packet_methods[PROT_CHG_USER_RESP_PACKET];
	DBG_VOID_RETURN;