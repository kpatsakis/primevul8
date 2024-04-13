static
size_t php_mysqlnd_cached_sha2_result_write(MYSQLND_CONN_DATA * conn, void * _packet)
{
	MYSQLND_PACKET_CACHED_SHA2_RESULT * packet= (MYSQLND_PACKET_CACHED_SHA2_RESULT *) _packet;
	MYSQLND_ERROR_INFO * error_info = conn->error_info;
	MYSQLND_PFC * pfc = conn->protocol_frame_codec;
	MYSQLND_VIO * vio = conn->vio;
	MYSQLND_STATS * stats = conn->stats;
	ALLOCA_FLAG(use_heap)
	zend_uchar *buffer = do_alloca(MYSQLND_HEADER_SIZE + packet->password_len + 1, use_heap);
	size_t sent;

	DBG_ENTER("php_mysqlnd_cached_sha2_result_write");

	if (packet->request == 1) {
		int1store(buffer + MYSQLND_HEADER_SIZE, '\2');
		sent = pfc->data->m.send(pfc, vio, buffer, 1, stats, error_info);
	} else {
		memcpy(buffer + MYSQLND_HEADER_SIZE, packet->password, packet->password_len);
		sent = pfc->data->m.send(pfc, vio, buffer, packet->password_len, stats, error_info);
	}

	free_alloca(buffer, use_heap);
	DBG_RETURN(sent);