void _xml_defaultHandler(void *userData, const XML_Char *s, int len)
{
	xml_parser *parser = (xml_parser *)userData;

	if (parser && !Z_ISUNDEF(parser->defaultHandler)) {
		zval retval, args[2];

		ZVAL_COPY(&args[0], &parser->index);
		_xml_xmlchar_zval(s, len, parser->target_encoding, &args[1]);
		xml_call_handler(parser, &parser->defaultHandler, parser->defaultPtr, 2, args, &retval);
		zval_ptr_dtor(&retval);
	}
}