static int spl_ptr_heap_cmp_cb_helper(zval *object, spl_heap_object *heap_object, zval *a, zval *b, long *result TSRMLS_DC) { /* {{{ */
		zval *result_p = NULL;

		zend_call_method_with_2_params(&object, heap_object->std.ce, &heap_object->fptr_cmp, "compare", &result_p, a, b);

		if (EG(exception)) {
			return FAILURE;
		}

		convert_to_long(result_p);
		*result = Z_LVAL_P(result_p);

		zval_ptr_dtor(&result_p);

		return SUCCESS;
}