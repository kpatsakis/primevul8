SPL_METHOD(SplPriorityQueue, compare)
{
	zval *a, *b;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &a, &b) == FAILURE) {
		return;
	}

	RETURN_LONG(spl_ptr_heap_zval_max_cmp(a, b, NULL TSRMLS_CC));
} 