static int curl_fnmatch(void *ctx, const char *pattern, const char *string)
{
	php_curl       *ch = (php_curl *) ctx;
	php_curl_fnmatch *t = ch->handlers->fnmatch;
	int rval = CURL_FNMATCHFUNC_FAIL;
	switch (t->method) {
		case PHP_CURL_USER: {
			zval **argv[3];
			zval  *zhandle = NULL;
			zval  *zpattern = NULL;
			zval  *zstring = NULL;
			zval  *retval_ptr;
			int   error;
			zend_fcall_info fci;
			TSRMLS_FETCH_FROM_CTX(ch->thread_ctx);

			MAKE_STD_ZVAL(zhandle);
			MAKE_STD_ZVAL(zpattern);
			MAKE_STD_ZVAL(zstring);

			ZVAL_RESOURCE(zhandle, ch->id);
			zend_list_addref(ch->id);
			ZVAL_STRING(zpattern, pattern, 1);
			ZVAL_STRING(zstring, string, 1);

			argv[0] = &zhandle;
			argv[1] = &zpattern;
			argv[2] = &zstring;

			fci.size = sizeof(fci);
			fci.function_table = EG(function_table);
			fci.function_name = t->func_name;
			fci.object_ptr = NULL;
			fci.retval_ptr_ptr = &retval_ptr;
			fci.param_count = 3;
			fci.params = argv;
			fci.no_separation = 0;
			fci.symbol_table = NULL;

			ch->in_callback = 1;
			error = zend_call_function(&fci, &t->fci_cache TSRMLS_CC);
			ch->in_callback = 0;
			if (error == FAILURE) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot call the CURLOPT_FNMATCH_FUNCTION");
			} else if (retval_ptr) {
				_php_curl_verify_handlers(ch, 1 TSRMLS_CC);
				if (Z_TYPE_P(retval_ptr) != IS_LONG) {
					convert_to_long_ex(&retval_ptr);
				}
				rval = Z_LVAL_P(retval_ptr);
				zval_ptr_dtor(&retval_ptr);
			}
			zval_ptr_dtor(argv[0]);
			zval_ptr_dtor(argv[1]);
			zval_ptr_dtor(argv[2]);
			break;
		}
	}
	return rval;
}