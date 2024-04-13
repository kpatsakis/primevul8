static enum_func_status
MYSQLND_METHOD(mysqlnd_protocol, send_command_handle_EOF)(
						MYSQLND_PROTOCOL_PAYLOAD_DECODER_FACTORY * const payload_decoder_factory,
						MYSQLND_ERROR_INFO * const error_info,
						MYSQLND_UPSERT_STATUS * const upsert_status)
{
	enum_func_status ret = FAIL;
	MYSQLND_PACKET_EOF response;

	payload_decoder_factory->m.init_eof_packet(&response);

	DBG_ENTER("mysqlnd_protocol::send_command_handle_EOF");

	if (FAIL == (ret = PACKET_READ(payload_decoder_factory->conn, &response))) {
		DBG_INF("Error while reading EOF packet");
		SET_CLIENT_ERROR(error_info, CR_MALFORMED_PACKET, UNKNOWN_SQLSTATE, "Malformed packet");
	} else if (0xFF == response.field_count) {
		/* The server signalled error. Set the error */
		DBG_INF_FMT("Error_no=%d SQLstate=%s Error=%s", response.error_no, response.sqlstate, response.error);

		SET_CLIENT_ERROR(error_info, response.error_no, response.sqlstate, response.error);

		UPSERT_STATUS_SET_AFFECTED_ROWS_TO_ERROR(upsert_status);
	} else if (0xFE != response.field_count) {
		SET_CLIENT_ERROR(error_info, CR_MALFORMED_PACKET, UNKNOWN_SQLSTATE, "Malformed packet");
		DBG_ERR_FMT("EOF packet expected, field count wasn't 0xFE but 0x%2X", response.field_count);
		php_error_docref(NULL, E_WARNING, "EOF packet expected, field count wasn't 0xFE but 0x%2X", response.field_count);
	} else {
		DBG_INF_FMT("EOF from server");
	}
	PACKET_FREE(&response);

	DBG_INF(ret == PASS ? "PASS":"FAIL");
	DBG_RETURN(ret);