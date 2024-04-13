void _xml_startNamespaceDeclHandler(void *userData,const XML_Char *prefix, const XML_Char *uri)
{
	xml_parser *parser = (xml_parser *)userData;

	if (parser && !Z_ISUNDEF(parser->startNamespaceDeclHandler)) {
		zval retval, args[3];

		ZVAL_COPY(&args[0], &parser->index);
		_xml_xmlchar_zval(prefix, 0, parser->target_encoding, &args[1]);
		_xml_xmlchar_zval(uri, 0, parser->target_encoding, &args[2]);
		xml_call_handler(parser, &parser->startNamespaceDeclHandler, parser->startNamespaceDeclPtr, 3, args, &retval);
		zval_ptr_dtor(&retval);
	}
}