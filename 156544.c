SPL_METHOD(SplHeap, next)
{
	spl_heap_object      *intern = (spl_heap_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	spl_ptr_heap_element  elem   = spl_ptr_heap_delete_top(intern->heap, getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (elem != NULL) {
		zval_ptr_dtor((zval **)&elem);
	}
}