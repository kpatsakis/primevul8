void _xml_processingInstructionHandler(void *userData, const XML_Char *target, const XML_Char *data)
{
	xml_parser *parser = (xml_parser *)userData;

	if (parser && !Z_ISUNDEF(parser->processingInstructionHandler)) {
		zval retval, args[3];

		ZVAL_COPY(&args[0], &parser->index);
		_xml_xmlchar_zval(target, 0, parser->target_encoding, &args[1]);
		_xml_xmlchar_zval(data, 0, parser->target_encoding, &args[2]);
		xml_call_handler(parser, &parser->processingInstructionHandler, parser->processingInstructionPtr, 3, args, &retval);
		zval_ptr_dtor(&retval);
	}
}