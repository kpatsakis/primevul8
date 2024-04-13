static void xml_call_handler(xml_parser *parser, zval *handler, zend_function *function_ptr, int argc, zval *argv, zval *retval)
{
	int i;	

	ZVAL_UNDEF(retval);
	if (parser && handler && !EG(exception)) {
		int result;
		zend_fcall_info fci;

		fci.size = sizeof(fci);
		fci.function_table = EG(function_table);
		ZVAL_COPY_VALUE(&fci.function_name, handler);
		fci.symbol_table = NULL;
		fci.object = Z_OBJ(parser->object);
		fci.retval = retval;
		fci.param_count = argc;
		fci.params = argv;
		fci.no_separation = 0;
		/*fci.function_handler_cache = &function_ptr;*/

		result = zend_call_function(&fci, NULL);
		if (result == FAILURE) {
			zval *method;
			zval *obj;

			if (Z_TYPE_P(handler) == IS_STRING) {
				php_error_docref(NULL, E_WARNING, "Unable to call handler %s()", Z_STRVAL_P(handler));
			} else if ((obj = zend_hash_index_find(Z_ARRVAL_P(handler), 0)) != NULL &&
					   (method = zend_hash_index_find(Z_ARRVAL_P(handler), 1)) != NULL &&
					   Z_TYPE_P(obj) == IS_OBJECT &&
					   Z_TYPE_P(method) == IS_STRING) {
				php_error_docref(NULL, E_WARNING, "Unable to call handler %s::%s()", ZSTR_VAL(Z_OBJCE_P(obj)->name), Z_STRVAL_P(method));
			} else 
				php_error_docref(NULL, E_WARNING, "Unable to call handler");
		}
	} 
	for (i = 0; i < argc; i++) {
		zval_ptr_dtor(&argv[i]);
	}
}