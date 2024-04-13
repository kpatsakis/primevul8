SPL_METHOD(SplHeap, current)
{
	spl_heap_object *intern  = (spl_heap_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	zval            *element = (zval *)intern->heap->elements[0];
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (!intern->heap->count || !element) {
		RETURN_NULL();
	} else {
		RETURN_ZVAL(element, 1, 0);
	}
}