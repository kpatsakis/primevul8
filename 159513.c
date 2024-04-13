static enum_func_status
php_mysqlnd_read_row_ex(MYSQLND_PFC * pfc,
						MYSQLND_VIO * vio,
						MYSQLND_STATS * stats,
						MYSQLND_ERROR_INFO * error_info,
						MYSQLND_CONNECTION_STATE * connection_state,
						MYSQLND_MEMORY_POOL * pool,
						MYSQLND_ROW_BUFFER * buffer,
						size_t * const data_size)
{
	enum_func_status ret = PASS;
	MYSQLND_PACKET_HEADER header;
	zend_uchar * p = NULL;
	size_t prealloc_more_bytes;

	DBG_ENTER("php_mysqlnd_read_row_ex");

	/*
	  To ease the process the server splits everything in packets up to 2^24 - 1.
	  Even in the case the payload is evenly divisible by this value, the last
	  packet will be empty, namely 0 bytes. Thus, we can read every packet and ask
	  for next one if they have 2^24 - 1 sizes. But just read the header of a
	  zero-length byte, don't read the body, there is no such.
	*/

	/*
	  We're allocating an extra byte, as php_mysqlnd_rowp_read_text_protocol_aux
	  needs to be able to append a terminating \0 for atoi/atof.
	*/
	prealloc_more_bytes = 1;

	*data_size = 0;
	if (UNEXPECTED(FAIL == mysqlnd_read_header(pfc, vio, &header, stats, error_info))) {
		ret = FAIL;
		SET_CONNECTION_STATE(connection_state, CONN_QUIT_SENT);
		set_packet_error(error_info, CR_SERVER_GONE_ERROR, UNKNOWN_SQLSTATE, mysqlnd_server_gone);
	} else {
		*data_size += header.size;
		buffer->ptr = pool->get_chunk(pool, *data_size + prealloc_more_bytes);
		p = buffer->ptr;

		if (UNEXPECTED(PASS != (ret = pfc->data->m.receive(pfc, vio, p, header.size, stats, error_info)))) {
			DBG_ERR("Empty row packet body");
			SET_CONNECTION_STATE(connection_state, CONN_QUIT_SENT);
			set_packet_error(error_info, CR_SERVER_GONE_ERROR, UNKNOWN_SQLSTATE, mysqlnd_server_gone);
		} else {
			while (header.size >= MYSQLND_MAX_PACKET_SIZE) {
				if (FAIL == mysqlnd_read_header(pfc, vio, &header, stats, error_info)) {
					ret = FAIL;
					break;
				}

				*data_size += header.size;

				/* Empty packet after MYSQLND_MAX_PACKET_SIZE packet. That's ok, break */
				if (!header.size) {
					break;
				}

				/*
				  We have to realloc the buffer.
				*/
				buffer->ptr = pool->resize_chunk(pool, buffer->ptr, *data_size - header.size, *data_size + prealloc_more_bytes);
				if (!buffer->ptr) {
					SET_OOM_ERROR(error_info);
					ret = FAIL;
					break;
				}
				/* The position could have changed, recalculate */
				p = (zend_uchar *) buffer->ptr + (*data_size - header.size);

				if (PASS != (ret = pfc->data->m.receive(pfc, vio, p, header.size, stats, error_info))) {
					DBG_ERR("Empty row packet body");
					SET_CONNECTION_STATE(connection_state, CONN_QUIT_SENT);
					set_packet_error(error_info, CR_SERVER_GONE_ERROR, UNKNOWN_SQLSTATE, mysqlnd_server_gone);
					break;
				}
			}
		}
	}
	if (ret == FAIL && buffer->ptr) {
		pool->free_chunk(pool, buffer->ptr);
		buffer->ptr = NULL;
	}
	DBG_RETURN(ret);