static enum_func_status
php_mysqlnd_cached_sha2_result_read(MYSQLND_CONN_DATA * conn, void * _packet)
{
	MYSQLND_PACKET_CACHED_SHA2_RESULT * packet= (MYSQLND_PACKET_CACHED_SHA2_RESULT *) _packet;
	MYSQLND_ERROR_INFO * error_info = conn->error_info;
	MYSQLND_PFC * pfc = conn->protocol_frame_codec;
	MYSQLND_VIO * vio = conn->vio;
	MYSQLND_STATS * stats = conn->stats;
	MYSQLND_CONNECTION_STATE * connection_state = &conn->state;
	zend_uchar buf[SHA256_PK_REQUEST_RESP_BUFFER_SIZE];
	zend_uchar *p = buf;
	const zend_uchar * const begin = buf;

	DBG_ENTER("php_mysqlnd_cached_sha2_result_read");
	if (FAIL == mysqlnd_read_packet_header_and_body(&(packet->header), pfc, vio, stats, error_info, connection_state, buf, sizeof(buf), "PROT_CACHED_SHA2_RESULT_PACKET", PROT_CACHED_SHA2_RESULT_PACKET)) {
		DBG_RETURN(FAIL);
	}
	BAIL_IF_NO_MORE_DATA;

	packet->response_code = uint1korr(p);
	p++;
	BAIL_IF_NO_MORE_DATA;

	if (ERROR_MARKER == packet->response_code) {
		php_mysqlnd_read_error_from_line(p, packet->header.size - 1,
										 packet->error, sizeof(packet->error),
										 &packet->error_no, packet->sqlstate
										);
		DBG_RETURN(PASS);
	}
	if (0xFE == packet->response_code) {
		/* Authentication Switch Response */
		if (packet->header.size > (size_t) (p - buf)) {
			packet->new_auth_protocol = mnd_pestrdup((char *)p, FALSE);
			packet->new_auth_protocol_len = strlen(packet->new_auth_protocol);
			p+= packet->new_auth_protocol_len + 1; /* +1 for the \0 */

			packet->new_auth_protocol_data_len = packet->header.size - (size_t) (p - buf);
			if (packet->new_auth_protocol_data_len) {
				packet->new_auth_protocol_data = mnd_emalloc(packet->new_auth_protocol_data_len);
				memcpy(packet->new_auth_protocol_data, p, packet->new_auth_protocol_data_len);
			}
			DBG_INF_FMT("The server requested switching auth plugin to : %s", packet->new_auth_protocol);
			DBG_INF_FMT("Server salt : [%d][%.*s]", packet->new_auth_protocol_data_len, packet->new_auth_protocol_data_len, packet->new_auth_protocol_data);
		}
		DBG_RETURN(PASS);
	}

	if (0x1 != packet->response_code) {
		DBG_ERR_FMT("Unexpected response code %d", packet->response_code);
	}

	/* This is not really the response code, but we reuse the field. */
	packet->response_code = uint1korr(p);
	p++;
	BAIL_IF_NO_MORE_DATA;

	packet->result = uint1korr(p);
	BAIL_IF_NO_MORE_DATA;

	DBG_RETURN(PASS);

premature_end:
	DBG_ERR_FMT("OK packet %d bytes shorter than expected", p - begin - packet->header.size);
	php_error_docref(NULL, E_WARNING, "SHA256_PK_REQUEST_RESPONSE packet "MYSQLND_SZ_T_SPEC" bytes shorter than expected",
					 p - begin - packet->header.size);
	DBG_RETURN(FAIL);