void _xml_endNamespaceDeclHandler(void *userData, const XML_Char *prefix)
{
	xml_parser *parser = (xml_parser *)userData;

	if (parser && !Z_ISUNDEF(parser->endNamespaceDeclHandler)) {
		zval retval, args[2];

		ZVAL_COPY(&args[0], &parser->index);
		_xml_xmlchar_zval(prefix, 0, parser->target_encoding, &args[1]);
		xml_call_handler(parser, &parser->endNamespaceDeclHandler, parser->endNamespaceDeclPtr, 2, args, &retval);
		zval_ptr_dtor(&retval);
	}
}