PHP_FUNCTION(xml_parser_free)
{
	zval *pind;
	xml_parser *parser;
	zend_resource *res;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &pind) == FAILURE) {
		return;
	}

	if ((parser = (xml_parser *)zend_fetch_resource(Z_RES_P(pind), "XML Parser", le_xml_parser)) == NULL) {
		RETURN_FALSE;
	}

	if (parser->isparsing == 1) {
		php_error_docref(NULL, E_WARNING, "Parser cannot be freed while it is parsing.");
		RETURN_FALSE;
	}

	res = Z_RES(parser->index);
	ZVAL_UNDEF(&parser->index);
	zend_list_close(res);
	RETURN_TRUE;
}