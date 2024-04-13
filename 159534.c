static enum_func_status
mysqlnd_read_packet_header_and_body(MYSQLND_PACKET_HEADER * packet_header,
									MYSQLND_PFC * pfc,
									MYSQLND_VIO * vio,
									MYSQLND_STATS * stats,
									MYSQLND_ERROR_INFO * error_info,
									MYSQLND_CONNECTION_STATE * connection_state,
									zend_uchar * const buf, const size_t buf_size,
									const char * const packet_type_as_text,
									enum mysqlnd_packet_type packet_type)
{
	DBG_ENTER("mysqlnd_read_packet_header_and_body");
	DBG_INF_FMT("buf=%p size=%u", buf, buf_size);
	if (FAIL == mysqlnd_read_header(pfc, vio, packet_header, stats, error_info)) {
		SET_CONNECTION_STATE(connection_state, CONN_QUIT_SENT);
		SET_CLIENT_ERROR(error_info, CR_SERVER_GONE_ERROR, UNKNOWN_SQLSTATE, mysqlnd_server_gone);
		DBG_ERR_FMT("Can't read %s's header", packet_type_as_text);
		DBG_RETURN(FAIL);
	}
	if (buf_size < packet_header->size) {
		DBG_ERR_FMT("Packet buffer %u wasn't big enough %u, %u bytes will be unread",
					buf_size, packet_header->size, packet_header->size - buf_size);
		DBG_RETURN(FAIL);
	}
	if (FAIL == pfc->data->m.receive(pfc, vio, buf, packet_header->size, stats, error_info)) {
		SET_CONNECTION_STATE(connection_state, CONN_QUIT_SENT);
		SET_CLIENT_ERROR(error_info, CR_SERVER_GONE_ERROR, UNKNOWN_SQLSTATE, mysqlnd_server_gone);
		DBG_ERR_FMT("Empty '%s' packet body", packet_type_as_text);
		DBG_RETURN(FAIL);
	}
	MYSQLND_INC_CONN_STATISTIC_W_VALUE2(stats, packet_type_to_statistic_byte_count[packet_type],
										MYSQLND_HEADER_SIZE + packet_header->size,
										packet_type_to_statistic_packet_count[packet_type],
										1);
	DBG_RETURN(PASS);