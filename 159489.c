static enum_func_status
php_mysqlnd_ok_read(MYSQLND_CONN_DATA * conn, void * _packet)
{
	register MYSQLND_PACKET_OK *packet= (MYSQLND_PACKET_OK *) _packet;
	MYSQLND_ERROR_INFO * error_info = conn->error_info;
	MYSQLND_PFC * pfc = conn->protocol_frame_codec;
	MYSQLND_VIO * vio = conn->vio;
	MYSQLND_STATS * stats = conn->stats;
	MYSQLND_CONNECTION_STATE * connection_state = &conn->state;
	zend_uchar local_buf[OK_BUFFER_SIZE];
	const size_t buf_len = pfc->cmd_buffer.buffer? pfc->cmd_buffer.length : OK_BUFFER_SIZE;
	zend_uchar * const buf = pfc->cmd_buffer.buffer? (zend_uchar *) pfc->cmd_buffer.buffer : local_buf;
	const zend_uchar * p = buf;
	const zend_uchar * const begin = buf;
	zend_ulong net_len;

	DBG_ENTER("php_mysqlnd_ok_read");

	if (FAIL == mysqlnd_read_packet_header_and_body(&(packet->header), pfc, vio, stats, error_info, connection_state, buf, buf_len, "OK", PROT_OK_PACKET)) {
		DBG_RETURN(FAIL);
	}
	BAIL_IF_NO_MORE_DATA;

	/* Should be always 0x0 or ERROR_MARKER for error */
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
	/* Everything was fine! */
	packet->affected_rows  = php_mysqlnd_net_field_length_ll(&p);
	BAIL_IF_NO_MORE_DATA;

	packet->last_insert_id = php_mysqlnd_net_field_length_ll(&p);
	BAIL_IF_NO_MORE_DATA;

	packet->server_status = uint2korr(p);
	p+= 2;
	BAIL_IF_NO_MORE_DATA;

	packet->warning_count = uint2korr(p);
	p+= 2;
	BAIL_IF_NO_MORE_DATA;

	/* There is a message */
	if (packet->header.size > (size_t) (p - buf) && (net_len = php_mysqlnd_net_field_length(&p))) {
		packet->message_len = MIN(net_len, buf_len - (p - begin));
		packet->message = mnd_pestrndup((char *)p, packet->message_len, FALSE);
	} else {
		packet->message = NULL;
		packet->message_len = 0;
	}

	DBG_INF_FMT("OK packet: aff_rows=%lld last_ins_id=%ld server_status=%u warnings=%u",
				packet->affected_rows, packet->last_insert_id, packet->server_status,
				packet->warning_count);

	BAIL_IF_NO_MORE_DATA;

	DBG_RETURN(PASS);
premature_end:
	DBG_ERR_FMT("OK packet %d bytes shorter than expected", p - begin - packet->header.size);
	php_error_docref(NULL, E_WARNING, "OK packet "MYSQLND_SZ_T_SPEC" bytes shorter than expected",
					 p - begin - packet->header.size);
	DBG_RETURN(FAIL);