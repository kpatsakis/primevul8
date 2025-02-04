PHP_FUNCTION(filter_input_array)
{
	long    fetch_from;
	zval   *array_input = NULL, **op = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|Z",  &fetch_from, &op) == FAILURE) {
		return;
	}

	if (op
		&& (Z_TYPE_PP(op) != IS_ARRAY)
		&& (Z_TYPE_PP(op) == IS_LONG && !PHP_FILTER_ID_EXISTS(Z_LVAL_PP(op)))
		) {
		RETURN_FALSE;
	}

	array_input = php_filter_get_storage(fetch_from TSRMLS_CC);

	if (!array_input || !HASH_OF(array_input)) {
		long filter_flags = 0;
		zval **option;
		if (op) {
			if (Z_TYPE_PP(op) == IS_LONG) {
				filter_flags = Z_LVAL_PP(op);
			} else if (Z_TYPE_PP(op) == IS_ARRAY && zend_hash_find(HASH_OF(*op), "flags", sizeof("flags"), (void **)&option) == SUCCESS) {
				PHP_FILTER_GET_LONG_OPT(option, filter_flags);
			}
		}
		if (filter_flags & FILTER_NULL_ON_FAILURE) {
			RETURN_FALSE;
		} else {
			RETURN_NULL();
		}
	}

	php_filter_array_handler(array_input, op, return_value TSRMLS_CC);
}