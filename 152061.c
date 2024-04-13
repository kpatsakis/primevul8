PHP_FUNCTION(utf8_encode)
{
	char *arg;
	size_t arg_len;
	zend_string *encoded;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	encoded = xml_utf8_encode(arg, arg_len, (XML_Char*)"ISO-8859-1");
	if (encoded == NULL) {
		RETURN_FALSE;
	}
	RETURN_STR(encoded);
}