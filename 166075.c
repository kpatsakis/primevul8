PHP_FUNCTION(curl_escape)
{
	char       *str = NULL, *res = NULL;
	int        str_len = 0;
	zval       *zid;
	php_curl   *ch;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &zid, &str, &str_len) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ch, php_curl *, &zid, -1, le_curl_name, le_curl);

	if ((res = curl_easy_escape(ch->cp, str, str_len))) {
		RETVAL_STRING(res, 1);
		curl_free(res);
	} else {
		RETURN_FALSE;
	}
}