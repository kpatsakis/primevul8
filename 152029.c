PHP_FUNCTION(xml_parse_into_struct)
{
	xml_parser *parser;
	zval *pind, *xdata, *info = NULL;
	char *data;
	size_t data_len;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rsz/|z/", &pind, &data, &data_len, &xdata, &info) == FAILURE) {
		return;
	}

	if (info) {	
		zval_ptr_dtor(info);
		array_init(info);
	}

	if ((parser = (xml_parser *)zend_fetch_resource(Z_RES_P(pind), "XML Parser", le_xml_parser)) == NULL) {
		RETURN_FALSE;
	}

	zval_ptr_dtor(xdata);
	array_init(xdata);

	ZVAL_COPY_VALUE(&parser->data, xdata);
	
	if (info) {
		ZVAL_COPY_VALUE(&parser->info, info);
	}
	
	parser->level = 0;
	parser->ltags = safe_emalloc(XML_MAXLEVEL, sizeof(char *), 0);

	XML_SetDefaultHandler(parser->parser, _xml_defaultHandler);
	XML_SetElementHandler(parser->parser, _xml_startElementHandler, _xml_endElementHandler);
	XML_SetCharacterDataHandler(parser->parser, _xml_characterDataHandler);

	parser->isparsing = 1;
	ret = XML_Parse(parser->parser, (XML_Char*)data, data_len, 1);
	parser->isparsing = 0;

	RETVAL_LONG(ret);
}