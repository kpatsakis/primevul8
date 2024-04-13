PHP_FUNCTION(curl_setopt_array)
{
	zval		*zid, *arr, **entry;
	php_curl	*ch;
	ulong		option;
	HashPosition	pos;
	char		*string_key;
	uint		str_key_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "za", &zid, &arr) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ch, php_curl *, &zid, -1, le_curl_name, le_curl);

	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(arr), &pos);
	while (zend_hash_get_current_data_ex(Z_ARRVAL_P(arr), (void **)&entry, &pos) == SUCCESS) {
		if (zend_hash_get_current_key_ex(Z_ARRVAL_P(arr), &string_key, &str_key_len, &option, 0, &pos) != HASH_KEY_IS_LONG) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Array keys must be CURLOPT constants or equivalent integer values");
			RETURN_FALSE;
		}
		if (_php_curl_setopt(ch, (long) option, entry TSRMLS_CC) == FAILURE) {
			RETURN_FALSE;
		}
		zend_hash_move_forward_ex(Z_ARRVAL_P(arr), &pos);
	}
	RETURN_TRUE;
}