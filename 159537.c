static enum_func_status
php_mysqlnd_eof_read(MYSQLND_CONN_DATA * conn, void * _packet)
{
	/*
	  EOF packet is since 4.1 five bytes long,
	  but we can get also an error, make it bigger.

	  Error : error_code + '#' + sqlstate + MYSQLND_ERRMSG_SIZE
	*/
	MYSQLND_PACKET_EOF *packet= (MYSQLND_PACKET_EOF *) _packet;
	MYSQLND_ERROR_INFO * error_info = conn->error_info;
	MYSQLND_PFC * pfc = conn->protocol_frame_codec;
	MYSQLND_VIO * vio = conn->vio;
	MYSQLND_STATS * stats = conn->stats;
	MYSQLND_CONNECTION_STATE * connection_state = &conn->state;
	const size_t buf_len = pfc->cmd_buffer.length;
	zend_uchar * const buf = (zend_uchar *) pfc->cmd_buffer.buffer;
	const zend_uchar * p = buf;
	const zend_uchar * const begin = buf;

	DBG_ENTER("php_mysqlnd_eof_read");

	if (FAIL == mysqlnd_read_packet_header_and_body(&(packet->header), pfc, vio, stats, error_info, connection_state, buf, buf_len, "EOF", PROT_EOF_PACKET)) {
		DBG_RETURN(FAIL);
	}
	BAIL_IF_NO_MORE_DATA;

	/* Should be always EODATA_MARKER */
	packet->field_count = uint1korr(p);
	p++;
	BAIL_IF_NO_MORE_DATA;

	if (ERROR_MARKER == packet->field_count) {
		php_mysqlnd_read_error_from_line(p, packet->header.size - 1,
										 packet->error, sizeof(packet->error),
										 &packet->error_no, packet->sqlstate
										);
		DBG_RETURN(PASS);
	}

	/*
		4.1 sends 1 byte EOF packet after metadata of
		PREPARE/EXECUTE but 5 bytes after the result. This is not
		according to the Docs@Forge!!!
	*/
	if (packet->header.size > 1) {
		packet->warning_count = uint2korr(p);
		p+= 2;
		BAIL_IF_NO_MORE_DATA;

		packet->server_status = uint2korr(p);
		p+= 2;
		BAIL_IF_NO_MORE_DATA;
	} else {
		packet->warning_count = 0;
		packet->server_status = 0;
	}

	BAIL_IF_NO_MORE_DATA;

	DBG_INF_FMT("EOF packet: fields=%u status=%u warnings=%u",
				packet->field_count, packet->server_status, packet->warning_count);

	DBG_RETURN(PASS);
premature_end:
	DBG_ERR_FMT("EOF packet %d bytes shorter than expected", p - begin - packet->header.size);
	php_error_docref(NULL, E_WARNING, "EOF packet "MYSQLND_SZ_T_SPEC" bytes shorter than expected",
					 p - begin - packet->header.size);
	DBG_RETURN(FAIL);