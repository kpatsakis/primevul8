static enum_func_status
php_mysqlnd_sha256_pk_request_response_read(MYSQLND_CONN_DATA * conn, void * _packet)
{
	MYSQLND_PACKET_SHA256_PK_REQUEST_RESPONSE * packet= (MYSQLND_PACKET_SHA256_PK_REQUEST_RESPONSE *) _packet;
	MYSQLND_ERROR_INFO * error_info = conn->error_info;
	MYSQLND_PFC * pfc = conn->protocol_frame_codec;
	MYSQLND_VIO * vio = conn->vio;
	MYSQLND_STATS * stats = conn->stats;
	MYSQLND_CONNECTION_STATE * connection_state = &conn->state;
	zend_uchar buf[SHA256_PK_REQUEST_RESP_BUFFER_SIZE];
	zend_uchar *p = buf;
	const zend_uchar * const begin = buf;

	DBG_ENTER("php_mysqlnd_sha256_pk_request_response_read");

	/* leave space for terminating safety \0 */
	if (FAIL == mysqlnd_read_packet_header_and_body(&(packet->header), pfc, vio, stats, error_info, connection_state, buf, sizeof(buf), "SHA256_PK_REQUEST_RESPONSE", PROT_SHA256_PK_REQUEST_RESPONSE_PACKET)) {
		DBG_RETURN(FAIL);
	}
	BAIL_IF_NO_MORE_DATA;

	p++;
	BAIL_IF_NO_MORE_DATA;

	packet->public_key_len = packet->header.size - (p - buf);
	packet->public_key = mnd_emalloc(packet->public_key_len + 1);
	memcpy(packet->public_key, p, packet->public_key_len);
	packet->public_key[packet->public_key_len] = '\0';

	DBG_RETURN(PASS);

premature_end:
	DBG_ERR_FMT("OK packet %d bytes shorter than expected", p - begin - packet->header.size);
	php_error_docref(NULL, E_WARNING, "SHA256_PK_REQUEST_RESPONSE packet "MYSQLND_SZ_T_SPEC" bytes shorter than expected",
					 p - begin - packet->header.size);
	DBG_RETURN(FAIL);