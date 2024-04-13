PHP_FUNCTION(utf8_decode)
{
	char *arg;
	size_t arg_len;
	zend_string *decoded;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	decoded = xml_utf8_decode((XML_Char*)arg, arg_len, (XML_Char*)"ISO-8859-1");
	if (decoded == NULL) {
		RETURN_FALSE;
	}
	RETURN_STR(decoded);
}