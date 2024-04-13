static enum_func_status
MYSQLND_METHOD(mysqlnd_protocol, send_command_handle_response)(
		MYSQLND_PROTOCOL_PAYLOAD_DECODER_FACTORY * payload_decoder_factory,
		const enum mysqlnd_packet_type ok_packet,
		const zend_bool silent,
		const enum php_mysqlnd_server_command command,
		const zend_bool ignore_upsert_status, /* actually used only by LOAD DATA. COM_QUERY and COM_EXECUTE handle the responses themselves */

		MYSQLND_ERROR_INFO	* error_info,
		MYSQLND_UPSERT_STATUS * upsert_status,
		MYSQLND_STRING * last_message
	)
{
	enum_func_status ret = FAIL;

	DBG_ENTER("mysqlnd_protocol::send_command_handle_response");
	DBG_INF_FMT("silent=%u packet=%u command=%s", silent, ok_packet, mysqlnd_command_to_text[command]);

	switch (ok_packet) {
		case PROT_OK_PACKET:
			ret = payload_decoder_factory->m.send_command_handle_OK(payload_decoder_factory, error_info, upsert_status, ignore_upsert_status, last_message);
			break;
		case PROT_EOF_PACKET:
			ret = payload_decoder_factory->m.send_command_handle_EOF(payload_decoder_factory, error_info, upsert_status);
			break;
		default:
			SET_CLIENT_ERROR(error_info, CR_MALFORMED_PACKET, UNKNOWN_SQLSTATE, "Malformed packet");
			php_error_docref(NULL, E_ERROR, "Wrong response packet %u passed to the function", ok_packet);
			break;
	}
	if (!silent && error_info->error_no == CR_MALFORMED_PACKET) {
		php_error_docref(NULL, E_WARNING, "Error while reading %s's response packet. PID=%d", mysqlnd_command_to_text[command], getpid());
	}
	DBG_INF(ret == PASS ? "PASS":"FAIL");
	DBG_RETURN(ret);