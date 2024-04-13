SPL_METHOD(SplHeap, extract)
{
	zval *value;
	spl_heap_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
		return;
	}

	intern = (spl_heap_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (intern->heap->flags & SPL_HEAP_CORRUPTED) {
		zend_throw_exception(spl_ce_RuntimeException, "Heap is corrupted, heap properties are no longer ensured.", 0 TSRMLS_CC);
		return;
	}

	value  = (zval *)spl_ptr_heap_delete_top(intern->heap, getThis() TSRMLS_CC);

	if (!value) {
		zend_throw_exception(spl_ce_RuntimeException, "Can't extract from an empty heap", 0 TSRMLS_CC);
		return;
	}

	RETURN_ZVAL(value, 1, 1);
} 