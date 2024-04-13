PHP_FUNCTION(xml_set_external_entity_ref_handler)
{
	xml_parser *parser;
	zval *pind, *hdl;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rz", &pind, &hdl) == FAILURE) {
		return;
	}

	if ((parser = (xml_parser *)zend_fetch_resource(Z_RES_P(pind), "XML Parser", le_xml_parser)) == NULL) {
		RETURN_FALSE;
	}

	xml_set_handler(&parser->externalEntityRefHandler, hdl);
	XML_SetExternalEntityRefHandler(parser->parser, (void *) _xml_externalEntityRefHandler);
	RETVAL_TRUE;
}