PHP_FUNCTION(xml_set_element_handler)
{
	xml_parser *parser;
	zval *pind, *shdl, *ehdl;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rzz", &pind, &shdl, &ehdl) == FAILURE) {
		return;
	}

	if ((parser = (xml_parser *)zend_fetch_resource(Z_RES_P(pind), "XML Parser", le_xml_parser)) == NULL) {
		RETURN_FALSE;
	}

	xml_set_handler(&parser->startElementHandler, shdl);
	xml_set_handler(&parser->endElementHandler, ehdl);
	XML_SetElementHandler(parser->parser, _xml_startElementHandler, _xml_endElementHandler);
	RETVAL_TRUE;
}