PHPAPI void
mysqlnd_protocol_payload_decoder_factory_free(MYSQLND_PROTOCOL_PAYLOAD_DECODER_FACTORY * const factory)
{
	DBG_ENTER("mysqlnd_protocol_payload_decoder_factory_free");

	if (factory) {
		zend_bool pers = factory->persistent;
		mnd_pefree(factory, pers);
	}
	DBG_VOID_RETURN;