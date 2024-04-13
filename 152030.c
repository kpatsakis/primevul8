PHP_FUNCTION(xml_get_current_byte_index)
{
	xml_parser *parser;
	zval *pind;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &pind) == FAILURE) {
		return;
	}

	if ((parser = (xml_parser *)zend_fetch_resource(Z_RES_P(pind), "XML Parser", le_xml_parser)) == NULL) {
		RETURN_FALSE;
	}

	RETVAL_LONG(XML_GetCurrentByteIndex(parser->parser));
}