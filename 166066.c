PHP_FUNCTION(curl_error)
{
	zval		*zid;
	php_curl	*ch;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zid) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ch, php_curl *, &zid, -1, le_curl_name, le_curl);

	ch->err.str[CURL_ERROR_SIZE] = 0;
	RETURN_STRING(ch->err.str, 1);
}