PHP_FUNCTION(curl_errno)
{
	zval		*zid;
	php_curl	*ch;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zid) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ch, php_curl *, &zid, -1, le_curl_name, le_curl);

	RETURN_LONG(ch->err.no);
}