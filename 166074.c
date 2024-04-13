PHP_FUNCTION(curl_strerror)
{
	long code;
	const char *str;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &code) == FAILURE) {
		return;
	}

	str = curl_easy_strerror(code);
	if (str) {
		RETURN_STRING(str, 1);
	} else {
		RETURN_NULL();
	}
}