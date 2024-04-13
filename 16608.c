static void msgfmt_do_format(MessageFormatter_object *mfo, zval *args, zval *return_value TSRMLS_DC)
{
	int count;
	UChar* formatted = NULL;
	int formatted_len = 0;
	HashTable *args_copy;

	count = zend_hash_num_elements(Z_ARRVAL_P(args));

	ALLOC_HASHTABLE(args_copy);
	zend_hash_init(args_copy, count, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_copy(args_copy, Z_ARRVAL_P(args), (copy_ctor_func_t)zval_add_ref,
		NULL, sizeof(zval*));

	umsg_format_helper(mfo, args_copy, &formatted, &formatted_len TSRMLS_CC);

	zend_hash_destroy(args_copy);
	efree(args_copy);

	if (U_FAILURE(INTL_DATA_ERROR_CODE(mfo))) {
		if (formatted) {
			efree(formatted);
		}
		RETURN_FALSE;
	} else {
		INTL_METHOD_RETVAL_UTF8(mfo, formatted, formatted_len, 1);
	}
}