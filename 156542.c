SPL_METHOD(SplHeap, count)
{
	long count;
	spl_heap_object *intern = (spl_heap_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
		return;
	}

	count = spl_ptr_heap_count(intern->heap);
	RETURN_LONG(count);
}