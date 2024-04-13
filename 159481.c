static void
MYSQLND_METHOD(mysqlnd_protocol, init_command_packet)(struct st_mysqlnd_packet_command *packet)
{
	DBG_ENTER("mysqlnd_protocol::init_command_packet");
	memset(packet, 0, sizeof(*packet));
	packet->header.m = &packet_methods[PROT_CMD_PACKET];
	DBG_VOID_RETURN;