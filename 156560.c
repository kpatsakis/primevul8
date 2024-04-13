SPL_METHOD(SplPriorityQueue, extract)
{
	zval *value, *value_out, **value_out_pp;
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

	value_out_pp = spl_pqueue_extract_helper(&value, intern->flags);


	if (!value_out_pp) {
		zend_error(E_RECOVERABLE_ERROR, "Unable to extract from the PriorityQueue node");
		zval_ptr_dtor(&value);
		return;
	}

	value_out = *value_out_pp;

	Z_ADDREF_P(value_out);
	zval_ptr_dtor(&value);

	RETURN_ZVAL(value_out, 1, 1);
} 