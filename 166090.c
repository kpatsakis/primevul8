PHP_FUNCTION(curl_init)
{
	php_curl	*ch;
	CURL		*cp;
	zval		*clone;
	char		*url = NULL;
	int		url_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &url, &url_len) == FAILURE) {
		return;
	}

	cp = curl_easy_init();
	if (!cp) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not initialize a new cURL handle");
		RETURN_FALSE;
	}

	alloc_curl_handle(&ch);
	TSRMLS_SET_CTX(ch->thread_ctx);

	ch->cp = cp;

	ch->handlers->write->method = PHP_CURL_STDOUT;
	ch->handlers->read->method  = PHP_CURL_DIRECT;
	ch->handlers->write_header->method = PHP_CURL_IGNORE;

	MAKE_STD_ZVAL(clone);
	ch->clone = clone;

	_php_curl_set_default_options(ch);

	if (url) {
		if (php_curl_option_url(ch, url, url_len TSRMLS_CC) == FAILURE) {
			_php_curl_close_ex(ch TSRMLS_CC);
			RETURN_FALSE;
		}
	}

	ZEND_REGISTER_RESOURCE(return_value, ch, le_curl);
	ch->id = Z_LVAL_P(return_value);
}