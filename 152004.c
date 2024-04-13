void _xml_notationDeclHandler(void *userData,
							  const XML_Char *notationName,
							  const XML_Char *base,
							  const XML_Char *systemId,
							  const XML_Char *publicId)
{
	xml_parser *parser = (xml_parser *)userData;

	if (parser && !Z_ISUNDEF(parser->notationDeclHandler)) {
		zval retval, args[5];

		ZVAL_COPY(&args[0], &parser->index);
		_xml_xmlchar_zval(notationName, 0, parser->target_encoding, &args[1]);
		_xml_xmlchar_zval(base, 0, parser->target_encoding, &args[2]);
		_xml_xmlchar_zval(systemId, 0, parser->target_encoding, &args[3]);
		_xml_xmlchar_zval(publicId, 0, parser->target_encoding, &args[4]);
		xml_call_handler(parser, &parser->notationDeclHandler, parser->notationDeclPtr, 5, args, &retval);
		zval_ptr_dtor(&retval);
	}
}