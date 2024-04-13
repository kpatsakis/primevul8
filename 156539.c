SPL_METHOD(SplHeap, recoverFromCorruption)
{
	spl_heap_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
		return;
	}

	intern = (spl_heap_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	intern->heap->flags = intern->heap->flags & ~SPL_HEAP_CORRUPTED;

	RETURN_TRUE;
}