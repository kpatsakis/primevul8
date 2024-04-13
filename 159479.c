static
size_t php_mysqlnd_auth_write(MYSQLND_CONN_DATA * conn, void * _packet)
{
	zend_uchar buffer[AUTH_WRITE_BUFFER_LEN];
	zend_uchar *p = buffer + MYSQLND_HEADER_SIZE; /* start after the header */
	size_t len;
	MYSQLND_PACKET_AUTH * packet= (MYSQLND_PACKET_AUTH *) _packet;
	MYSQLND_ERROR_INFO * error_info = conn->error_info;
	MYSQLND_PFC * pfc = conn->protocol_frame_codec;
	MYSQLND_VIO * vio = conn->vio;
	MYSQLND_STATS * stats = conn->stats;
	MYSQLND_CONNECTION_STATE * connection_state = &conn->state;

	DBG_ENTER("php_mysqlnd_auth_write");

	if (!packet->is_change_user_packet) {
		int4store(p, packet->client_flags);
		p+= 4;

		int4store(p, packet->max_packet_size);
		p+= 4;

		int1store(p, packet->charset_no);
		p++;

		memset(p, 0, 23); /* filler */
		p+= 23;
	}

	if (packet->send_auth_data || packet->is_change_user_packet) {
		len = MIN(strlen(packet->user), MYSQLND_MAX_ALLOWED_USER_LEN);
		memcpy(p, packet->user, len);
		p+= len;
		*p++ = '\0';

		/* defensive coding */
		if (packet->auth_data == NULL) {
			packet->auth_data_len = 0;
		}
		if (packet->auth_data_len > 0xFF) {
			const char * const msg = "Authentication data too long. "
				"Won't fit into the buffer and will be truncated. Authentication will thus fail";
			SET_CLIENT_ERROR(error_info, CR_UNKNOWN_ERROR, UNKNOWN_SQLSTATE, msg);
			php_error_docref(NULL, E_WARNING, "%s", msg);
			DBG_RETURN(0);
		}

		int1store(p, (int8_t)packet->auth_data_len);
		++p;
/*!!!!! is the buffer big enough ??? */
		if (sizeof(buffer) < (packet->auth_data_len + (p - buffer))) {
			DBG_ERR("the stack buffer was not enough!!");
			DBG_RETURN(0);
		}
		if (packet->auth_data_len) {
			memcpy(p, packet->auth_data, packet->auth_data_len);
			p+= packet->auth_data_len;
		}

		if (packet->db_len > 0) {
			/* CLIENT_CONNECT_WITH_DB should have been set */
			size_t real_db_len = MIN(MYSQLND_MAX_ALLOWED_DB_LEN, packet->db_len);
			memcpy(p, packet->db, real_db_len);
			p+= real_db_len;
			*p++= '\0';
		} else if (packet->is_change_user_packet) {
			*p++= '\0';
		}
		/* no \0 for no DB */

		if (packet->is_change_user_packet) {
			if (packet->charset_no) {
				int2store(p, packet->charset_no);
				p+= 2;
			}
		}

		if (packet->auth_plugin_name) {
			len = MIN(strlen(packet->auth_plugin_name), sizeof(buffer) - (p - buffer) - 1);
			memcpy(p, packet->auth_plugin_name, len);
			p+= len;
			*p++= '\0';
		}

		if (packet->connect_attr && zend_hash_num_elements(packet->connect_attr)) {
			size_t ca_payload_len = 0;

			{
				zend_string * key;
				zval * entry_value;
				ZEND_HASH_FOREACH_STR_KEY_VAL(packet->connect_attr, key, entry_value) {
					if (key) { /* HASH_KEY_IS_STRING */
						size_t value_len = Z_STRLEN_P(entry_value);

						ca_payload_len += php_mysqlnd_net_store_length_size(ZSTR_LEN(key));
						ca_payload_len += ZSTR_LEN(key);
						ca_payload_len += php_mysqlnd_net_store_length_size(value_len);
						ca_payload_len += value_len;
					}
				} ZEND_HASH_FOREACH_END();
			}

			if (sizeof(buffer) >= (ca_payload_len + php_mysqlnd_net_store_length_size(ca_payload_len) + (p - buffer))) {
				p = php_mysqlnd_net_store_length(p, ca_payload_len);

				{
					zend_string * key;
					zval * entry_value;
					ZEND_HASH_FOREACH_STR_KEY_VAL(packet->connect_attr, key, entry_value) {
						if (key) { /* HASH_KEY_IS_STRING */
							size_t value_len = Z_STRLEN_P(entry_value);

							/* copy key */
							p = php_mysqlnd_net_store_length(p, ZSTR_LEN(key));
							memcpy(p, ZSTR_VAL(key), ZSTR_LEN(key));
							p+= ZSTR_LEN(key);
							/* copy value */
							p = php_mysqlnd_net_store_length(p, value_len);
							memcpy(p, Z_STRVAL_P(entry_value), value_len);
							p+= value_len;
						}
					} ZEND_HASH_FOREACH_END();
				}
			} else {
				/* cannot put the data - skip */
			}
		}
	}
	if (packet->is_change_user_packet) {
		enum_func_status ret = FAIL;
		const MYSQLND_CSTRING payload = {(char*) buffer + MYSQLND_HEADER_SIZE, p - (buffer + MYSQLND_HEADER_SIZE)};
		const unsigned int silent = packet->silent;

		ret = conn->command->change_user(conn, payload, silent);
		DBG_RETURN(ret == PASS? (p - buffer - MYSQLND_HEADER_SIZE) : 0);
	} else {
		/*
		  The auth handshake packet has no command in it. Thus we can't go over conn->command directly.
		  Well, we can have a command->no_command(conn, payload)
		*/
		const size_t sent = pfc->data->m.send(pfc, vio, buffer, p - buffer - MYSQLND_HEADER_SIZE, stats, error_info);
		if (!sent) {
			SET_CONNECTION_STATE(connection_state, CONN_QUIT_SENT);
		}
		DBG_RETURN(sent);
	}