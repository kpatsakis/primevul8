static int spl_ptr_pqueue_zval_cmp(spl_ptr_heap_element a, spl_ptr_heap_element b, void* object TSRMLS_DC) { /* {{{ */
	zval result;
	zval **a_priority_pp = spl_pqueue_extract_helper((zval **)&a, SPL_PQUEUE_EXTR_PRIORITY);
	zval **b_priority_pp = spl_pqueue_extract_helper((zval **)&b, SPL_PQUEUE_EXTR_PRIORITY);

	if ((!a_priority_pp) || (!b_priority_pp)) {
		zend_error(E_RECOVERABLE_ERROR, "Unable to extract from the PriorityQueue node");
		return 0;
	}
	if (EG(exception)) {
		return 0;
	}

	if (object) {
		spl_heap_object *heap_object = (spl_heap_object*)zend_object_store_get_object(object TSRMLS_CC);
		if (heap_object->fptr_cmp) {
			long lval = 0;
			if (spl_ptr_heap_cmp_cb_helper((zval *)object, heap_object, *a_priority_pp, *b_priority_pp, &lval TSRMLS_CC) == FAILURE) {
				/* exception or call failure */
				return 0;
			}
			return lval;
		}
	}

	INIT_ZVAL(result);
	compare_function(&result, *a_priority_pp, *b_priority_pp TSRMLS_CC);
	return Z_LVAL(result);
}