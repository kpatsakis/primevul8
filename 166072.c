PHP_FUNCTION(curl_close)
{
	zval		*zid;
	php_curl	*ch;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zid) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ch, php_curl *, &zid, -1, le_curl_name, le_curl);

	if (ch->in_callback) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Attempt to close cURL handle from a callback");
		return;
	}

	zend_list_delete(Z_LVAL_P(zid));
}