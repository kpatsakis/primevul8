static
size_t php_mysqlnd_sha256_pk_request_write(MYSQLND_CONN_DATA * conn, void * _packet)
{
	MYSQLND_ERROR_INFO * error_info = conn->error_info;
	MYSQLND_PFC * pfc = conn->protocol_frame_codec;
	MYSQLND_VIO * vio = conn->vio;
	MYSQLND_STATS * stats = conn->stats;
	zend_uchar buffer[MYSQLND_HEADER_SIZE + 1];
	size_t sent;

	DBG_ENTER("php_mysqlnd_sha256_pk_request_write");

	int1store(buffer + MYSQLND_HEADER_SIZE, '\1');
	sent = pfc->data->m.send(pfc, vio, buffer, 1, stats, error_info);

	DBG_RETURN(sent);