int _xml_externalEntityRefHandler(XML_Parser parserPtr,
								   const XML_Char *openEntityNames,
								   const XML_Char *base,
								   const XML_Char *systemId,
								   const XML_Char *publicId)
{
	xml_parser *parser = XML_GetUserData(parserPtr);
	int ret = 0; /* abort if no handler is set (should be configurable?) */

	if (parser && !Z_ISUNDEF(parser->externalEntityRefHandler)) {
		zval retval, args[5];

		ZVAL_COPY(&args[0], &parser->index);
		_xml_xmlchar_zval(openEntityNames, 0, parser->target_encoding, &args[1]);
		_xml_xmlchar_zval(base, 0, parser->target_encoding, &args[2]);
		_xml_xmlchar_zval(systemId, 0, parser->target_encoding, &args[3]);
		_xml_xmlchar_zval(publicId, 0, parser->target_encoding, &args[4]);
		xml_call_handler(parser, &parser->externalEntityRefHandler, parser->externalEntityRefPtr, 5, args, &retval);
		if (!Z_ISUNDEF(retval)) {
			convert_to_long(&retval);
			ret = Z_LVAL(retval);
		} else {
			ret = 0;
		}
	}
	return ret;
}