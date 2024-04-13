PHP_FUNCTION(xml_parser_get_option)
{
	xml_parser *parser;
	zval *pind;
	zend_long opt;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl", &pind, &opt) == FAILURE) {
		return;
	}

	if ((parser = (xml_parser *)zend_fetch_resource(Z_RES_P(pind), "XML Parser", le_xml_parser)) == NULL) {
		RETURN_FALSE;
	}

	switch (opt) {
		case PHP_XML_OPTION_CASE_FOLDING:
			RETURN_LONG(parser->case_folding);
			break;
		case PHP_XML_OPTION_TARGET_ENCODING:
			RETURN_STRING((char *)parser->target_encoding);
			break;
		default:
			php_error_docref(NULL, E_WARNING, "Unknown option");
			RETURN_FALSE;
			break;
	}

	RETVAL_FALSE;	/* never reached */
}