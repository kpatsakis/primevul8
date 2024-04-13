PHP_FUNCTION(curl_unescape)
{
	char       *str = NULL, *out = NULL;
	int        str_len = 0, out_len;
	zval       *zid;
	php_curl   *ch;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &zid, &str, &str_len) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ch, php_curl *, &zid, -1, le_curl_name, le_curl);

	if ((out = curl_easy_unescape(ch->cp, str, str_len, &out_len))) {
		RETVAL_STRINGL(out, out_len, 1);
		curl_free(out);
	} else {
		RETURN_FALSE;
	}
}