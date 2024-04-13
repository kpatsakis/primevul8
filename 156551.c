SPL_METHOD(SplHeap, insert)
{
	zval *value;
	spl_heap_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &value) == FAILURE) {
		return;
	}

	intern = (spl_heap_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (intern->heap->flags & SPL_HEAP_CORRUPTED) {
		zend_throw_exception(spl_ce_RuntimeException, "Heap is corrupted, heap properties are no longer ensured.", 0 TSRMLS_CC);
		return;
	}

	SEPARATE_ARG_IF_REF(value);

	spl_ptr_heap_insert(intern->heap, value, getThis() TSRMLS_CC);

	RETURN_TRUE;
} 