PHPAPI MYSQLND_PROTOCOL_PAYLOAD_DECODER_FACTORY *
mysqlnd_protocol_payload_decoder_factory_init(MYSQLND_CONN_DATA * conn, const zend_bool persistent)
{
	MYSQLND_PROTOCOL_PAYLOAD_DECODER_FACTORY * ret;
	DBG_ENTER("mysqlnd_protocol_payload_decoder_factory_init");
	ret = MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_object_factory).get_protocol_payload_decoder_factory(conn, persistent);
	DBG_RETURN(ret);