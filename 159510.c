static enum_func_status
php_mysqlnd_rset_header_read(MYSQLND_CONN_DATA * conn, void * _packet)
{
	MYSQLND_PACKET_RSET_HEADER * packet= (MYSQLND_PACKET_RSET_HEADER *) _packet;
	MYSQLND_ERROR_INFO * error_info = conn->error_info;
	MYSQLND_PFC * pfc = conn->protocol_frame_codec;
	MYSQLND_VIO * vio = conn->vio;
	MYSQLND_STATS * stats = conn->stats;
	MYSQLND_CONNECTION_STATE * connection_state = &conn->state;
	enum_func_status ret = PASS;
	const size_t buf_len = pfc->cmd_buffer.length;
	zend_uchar * const buf = (zend_uchar *) pfc->cmd_buffer.buffer;
	const zend_uchar * p = buf;
	const zend_uchar * const begin = buf;
	size_t len;

	DBG_ENTER("php_mysqlnd_rset_header_read");

	if (FAIL == mysqlnd_read_packet_header_and_body(&(packet->header), pfc, vio, stats, error_info, connection_state, buf, buf_len, "resultset header", PROT_RSET_HEADER_PACKET)) {
		DBG_RETURN(FAIL);
	}
	BAIL_IF_NO_MORE_DATA;

	/*
	  Don't increment. First byte is ERROR_MARKER on error, but otherwise is starting byte
	  of encoded sequence for length.
	*/
	if (ERROR_MARKER == *p) {
		/* Error */
		p++;
		BAIL_IF_NO_MORE_DATA;
		php_mysqlnd_read_error_from_line(p, packet->header.size - 1,
										 packet->error_info.error, sizeof(packet->error_info.error),
										 &packet->error_info.error_no, packet->error_info.sqlstate
										);
		DBG_RETURN(PASS);
	}

	packet->field_count = php_mysqlnd_net_field_length(&p);
	BAIL_IF_NO_MORE_DATA;

	switch (packet->field_count) {
		case MYSQLND_NULL_LENGTH:
			DBG_INF("LOAD LOCAL");
			/*
			  First byte in the packet is the field count.
			  Thus, the name is size - 1. And we add 1 for a trailing \0.
			  Because we have BAIL_IF_NO_MORE_DATA before the switch, we are guaranteed
			  that packet->header.size is > 0. Which means that len can't underflow, that
			  would lead to 0 byte allocation but 2^32 or 2^64 bytes copied.
			*/
			len = packet->header.size - 1;
			packet->info_or_local_file.s = mnd_emalloc(len + 1);
			if (packet->info_or_local_file.s) {
				memcpy(packet->info_or_local_file.s, p, len);
				packet->info_or_local_file.s[len] = '\0';
				packet->info_or_local_file.l = len;
			} else {
				SET_OOM_ERROR(error_info);
				ret = FAIL;
			}
			break;
		case 0x00:
			DBG_INF("UPSERT");
			packet->affected_rows = php_mysqlnd_net_field_length_ll(&p);
			BAIL_IF_NO_MORE_DATA;

			packet->last_insert_id = php_mysqlnd_net_field_length_ll(&p);
			BAIL_IF_NO_MORE_DATA;

			packet->server_status = uint2korr(p);
			p+=2;
			BAIL_IF_NO_MORE_DATA;

			packet->warning_count = uint2korr(p);
			p+=2;
			BAIL_IF_NO_MORE_DATA;
			/* Check for additional textual data */
			if (packet->header.size  > (size_t) (p - buf) && (len = php_mysqlnd_net_field_length(&p))) {
				packet->info_or_local_file.s = mnd_emalloc(len + 1);
				if (packet->info_or_local_file.s) {
					memcpy(packet->info_or_local_file.s, p, len);
					packet->info_or_local_file.s[len] = '\0';
					packet->info_or_local_file.l = len;
				} else {
					SET_OOM_ERROR(error_info);
					ret = FAIL;
				}
			}
			DBG_INF_FMT("affected_rows=%llu last_insert_id=%llu server_status=%u warning_count=%u",
						packet->affected_rows, packet->last_insert_id,
						packet->server_status, packet->warning_count);
			break;
		default:
			DBG_INF("SELECT");
			/* Result set */
			break;
	}
	BAIL_IF_NO_MORE_DATA;

	DBG_RETURN(ret);
premature_end:
	DBG_ERR_FMT("RSET_HEADER packet %d bytes shorter than expected", p - begin - packet->header.size);
	php_error_docref(NULL, E_WARNING, "RSET_HEADER packet "MYSQLND_SZ_T_SPEC" bytes shorter than expected",
					 p - begin - packet->header.size);
	DBG_RETURN(FAIL);