PHP_FUNCTION(xml_set_unparsed_entity_decl_handler)
{
	xml_parser *parser;
	zval *pind, *hdl;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rz", &pind, &hdl) == FAILURE) {
		return;
	}

	if ((parser = (xml_parser *)zend_fetch_resource(Z_RES_P(pind), "XML Parser", le_xml_parser)) == NULL) {
		RETURN_FALSE;
	}

	xml_set_handler(&parser->unparsedEntityDeclHandler, hdl);
	XML_SetUnparsedEntityDeclHandler(parser->parser, _xml_unparsedEntityDeclHandler);
	RETVAL_TRUE;
}