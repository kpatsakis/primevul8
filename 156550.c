static int spl_ptr_heap_zval_max_cmp(spl_ptr_heap_element a, spl_ptr_heap_element b, void* object TSRMLS_DC) { /* {{{ */
	zval result;

	if (EG(exception)) {
		return 0;
	}

	if (object) {
		spl_heap_object *heap_object = (spl_heap_object*)zend_object_store_get_object((zval *)object TSRMLS_CC);
		if (heap_object->fptr_cmp) {
			long lval = 0;
			if (spl_ptr_heap_cmp_cb_helper((zval *)object, heap_object, (zval *)a, (zval *)b, &lval TSRMLS_CC) == FAILURE) {
				/* exception or call failure */
				return 0;
			}
			return lval;
		}
	}

	INIT_ZVAL(result);
	compare_function(&result, (zval *)a, (zval *)b TSRMLS_CC);
	return Z_LVAL(result);
}