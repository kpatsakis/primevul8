PHP_FUNCTION(xml_parse)
{
	xml_parser *parser;
	zval *pind;
	char *data;
	size_t data_len;
	int ret;
	zend_bool isFinal = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs|b", &pind, &data, &data_len, &isFinal) == FAILURE) {
		return;
	}

	if ((parser = (xml_parser *)zend_fetch_resource(Z_RES_P(pind), "XML Parser", le_xml_parser)) == NULL) {
		RETURN_FALSE;
	}

	parser->isparsing = 1;
	ret = XML_Parse(parser->parser, (XML_Char*)data, data_len, isFinal);
	parser->isparsing = 0;
	RETVAL_LONG(ret);
}