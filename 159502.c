static enum_func_status
MYSQLND_METHOD(mysqlnd_protocol, send_command_handle_OK)(
						MYSQLND_PROTOCOL_PAYLOAD_DECODER_FACTORY * const payload_decoder_factory,
						MYSQLND_ERROR_INFO * const error_info,
						MYSQLND_UPSERT_STATUS * const upsert_status,
						const zend_bool ignore_upsert_status,  /* actually used only by LOAD DATA. COM_QUERY and COM_EXECUTE handle the responses themselves */
						MYSQLND_STRING * const last_message)
{
	enum_func_status ret = FAIL;
	MYSQLND_PACKET_OK ok_response;

	payload_decoder_factory->m.init_ok_packet(&ok_response);
	DBG_ENTER("mysqlnd_protocol::send_command_handle_OK");
	if (FAIL == (ret = PACKET_READ(payload_decoder_factory->conn, &ok_response))) {
		DBG_INF("Error while reading OK packet");
		SET_CLIENT_ERROR(error_info, CR_MALFORMED_PACKET, UNKNOWN_SQLSTATE, "Malformed packet");
		goto end;
	}
	DBG_INF_FMT("OK from server");
	if (0xFF == ok_response.field_count) {
		/* The server signalled error. Set the error */
		SET_CLIENT_ERROR(error_info, ok_response.error_no, ok_response.sqlstate, ok_response.error);
		ret = FAIL;
		/*
		  Cover a protocol design error: error packet does not
		  contain the server status. Therefore, the client has no way
		  to find out whether there are more result sets of
		  a multiple-result-set statement pending. Luckily, in 5.0 an
		  error always aborts execution of a statement, wherever it is
		  a multi-statement or a stored procedure, so it should be
		  safe to unconditionally turn off the flag here.
		*/
		upsert_status->server_status &= ~SERVER_MORE_RESULTS_EXISTS;
		UPSERT_STATUS_SET_AFFECTED_ROWS_TO_ERROR(upsert_status);
	} else {
		SET_NEW_MESSAGE(last_message->s, last_message->l,
						ok_response.message, ok_response.message_len);
		if (!ignore_upsert_status) {
			UPSERT_STATUS_RESET(upsert_status);
			UPSERT_STATUS_SET_WARNINGS(upsert_status, ok_response.warning_count);
			UPSERT_STATUS_SET_SERVER_STATUS(upsert_status, ok_response.server_status);
			UPSERT_STATUS_SET_AFFECTED_ROWS(upsert_status, ok_response.affected_rows);
			UPSERT_STATUS_SET_LAST_INSERT_ID(upsert_status, ok_response.last_insert_id);
		} else {
			/* LOAD DATA */
		}
	}
end:
	PACKET_FREE(&ok_response);
	DBG_INF(ret == PASS ? "PASS":"FAIL");
	DBG_RETURN(ret);