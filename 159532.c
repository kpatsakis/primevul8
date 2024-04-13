static enum_func_status
php_mysqlnd_stats_read(MYSQLND_CONN_DATA * conn, void * _packet)
{
	MYSQLND_PACKET_STATS *packet= (MYSQLND_PACKET_STATS *) _packet;
	MYSQLND_ERROR_INFO * error_info = conn->error_info;
	MYSQLND_PFC * pfc = conn->protocol_frame_codec;
	MYSQLND_VIO * vio = conn->vio;
	MYSQLND_STATS * stats = conn->stats;
	MYSQLND_CONNECTION_STATE * connection_state = &conn->state;
	const size_t buf_len = pfc->cmd_buffer.length;
	zend_uchar *buf = (zend_uchar *) pfc->cmd_buffer.buffer;

	DBG_ENTER("php_mysqlnd_stats_read");

	if (FAIL == mysqlnd_read_packet_header_and_body(&(packet->header), pfc, vio, stats, error_info, connection_state, buf, buf_len, "statistics", PROT_STATS_PACKET)) {
		DBG_RETURN(FAIL);
	}

	packet->message.s = mnd_emalloc(packet->header.size + 1);
	memcpy(packet->message.s, buf, packet->header.size);
	packet->message.s[packet->header.size] = '\0';
	packet->message.l = packet->header.size;

	DBG_RETURN(PASS);