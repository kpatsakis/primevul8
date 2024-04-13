static size_t curl_passwd(void *ctx, char *prompt, char *buf, int buflen)
{
	php_curl    *ch   = (php_curl *) ctx;
	zval        *func = ch->handlers->passwd;
	zval        *argv[3];
	zval        *retval = NULL;
	int          error;
	int          ret = -1;
	TSRMLS_FETCH_FROM_CTX(ch->thread_ctx);

	MAKE_STD_ZVAL(argv[0]);
	MAKE_STD_ZVAL(argv[1]);
	MAKE_STD_ZVAL(argv[2]);

	ZVAL_RESOURCE(argv[0], ch->id);
	zend_list_addref(ch->id);
	ZVAL_STRING(argv[1], prompt, 1);
	ZVAL_LONG(argv[2], buflen);

	error = call_user_function(EG(function_table), NULL, func, retval, 2, argv TSRMLS_CC);
	if (error == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not call the CURLOPT_PASSWDFUNCTION");
	} else if (Z_TYPE_P(retval) == IS_STRING) {
		if (Z_STRLEN_P(retval) > buflen) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Returned password is too long for libcurl to handle");
		} else {
			strlcpy(buf, Z_STRVAL_P(retval), Z_STRLEN_P(retval));
		}
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "User handler '%s' did not return a string", Z_STRVAL_P(func));
	}

	zval_ptr_dtor(&argv[0]);
	zval_ptr_dtor(&argv[1]);
	zval_ptr_dtor(&argv[2]);
	zval_ptr_dtor(&retval);

	return ret;
}