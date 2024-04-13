static enum_func_status
php_mysqlnd_rowp_read(MYSQLND_CONN_DATA * conn, void * _packet)
{
	MYSQLND_PACKET_ROW *packet = (MYSQLND_PACKET_ROW *) _packet;
	MYSQLND_ERROR_INFO * error_info = &packet->error_info;
	MYSQLND_PFC * pfc = conn->protocol_frame_codec;
	MYSQLND_VIO * vio = conn->vio;
	MYSQLND_STATS * stats = conn->stats;
	zend_uchar *p;
	enum_func_status ret = PASS;
	size_t data_size = 0;

	DBG_ENTER("php_mysqlnd_rowp_read");

	ret = php_mysqlnd_read_row_ex(pfc, vio, stats, error_info, &conn->state,
								  packet->result_set_memory_pool, &packet->row_buffer, &data_size);
	if (FAIL == ret) {
		goto end;
	}
	MYSQLND_INC_CONN_STATISTIC_W_VALUE2(stats, packet_type_to_statistic_byte_count[PROT_ROW_PACKET],
										MYSQLND_HEADER_SIZE + packet->header.size,
										packet_type_to_statistic_packet_count[PROT_ROW_PACKET],
										1);

	/*
	  packet->row_buffer->ptr is of size 'data_size'
	  in pre-7.0 it was really 'data_size + 1' although it was counted as 'data_size'
	  The +1 was for the additional byte needed to \0 terminate the last string in the row.
	  This was needed as the zvals of pre-7.0 could use external memory (no copy param to ZVAL_STRINGL).
	  However, in 7.0+ the strings always copy. Thus this +1 byte was removed. Also the optimization or \0
	  terminating every string, which did overwrite the lengths from the packet. For this reason we needed
	  to keep (and copy) the lengths externally.
	*/
	packet->header.size = data_size;
	packet->row_buffer.size = data_size;

	if (ERROR_MARKER == (*(p = packet->row_buffer.ptr))) {
		/*
		   Error message as part of the result set,
		   not good but we should not hang. See:
		   Bug #27876 : SF with cyrillic variable name fails during execution
		*/
		ret = FAIL;
		php_mysqlnd_read_error_from_line(p + 1, data_size - 1,
										 packet->error_info.error,
										 sizeof(packet->error_info.error),
										 &packet->error_info.error_no,
										 packet->error_info.sqlstate
										);
	} else if (EODATA_MARKER == *p && data_size < 8) { /* EOF */
		packet->eof = TRUE;
		p++;
		if (data_size > 1) {
			packet->warning_count = uint2korr(p);
			p += 2;
			packet->server_status = uint2korr(p);
			/* Seems we have 3 bytes reserved for future use */
			DBG_INF_FMT("server_status=%u warning_count=%u", packet->server_status, packet->warning_count);
		}
	} else {
		MYSQLND_INC_CONN_STATISTIC(stats,
									packet->binary_protocol? STAT_ROWS_FETCHED_FROM_SERVER_PS:
															 STAT_ROWS_FETCHED_FROM_SERVER_NORMAL);

		packet->eof = FALSE;
		/* packet->field_count is set by the user of the packet */

		if (!packet->skip_extraction) {
			if (!packet->fields) {
				DBG_INF("Allocating packet->fields");
				/*
				  old-API will probably set packet->fields to NULL every time, though for
				  unbuffered sets it makes not much sense as the zvals in this buffer matter,
				  not the buffer. Constantly allocating and deallocating brings nothing.

				  For PS - if stmt_store() is performed, thus we don't have a cursor, it will
				  behave just like old-API buffered. Cursors will behave like a bit different,
				  but mostly like old-API unbuffered and thus will populate this array with
				  value.
				*/
				packet->fields = mnd_ecalloc(packet->field_count, sizeof(zval));
			}
		} else {
			MYSQLND_INC_CONN_STATISTIC(stats,
										packet->binary_protocol? STAT_ROWS_SKIPPED_PS:
																 STAT_ROWS_SKIPPED_NORMAL);
		}
	}

end:
	DBG_RETURN(ret);