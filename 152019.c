static zend_string *_xml_decode_tag(xml_parser *parser, const char *tag)
{
	zend_string *str;

	str = xml_utf8_decode((const XML_Char *)tag, strlen(tag), parser->target_encoding);

	if (parser->case_folding) {
		php_strtoupper(ZSTR_VAL(str), ZSTR_LEN(str));
	}

	return str;
}