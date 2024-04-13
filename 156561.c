SPL_METHOD(SplPriorityQueue, current)
{
	spl_heap_object  *intern  = (spl_heap_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	zval            **element = (zval **)&intern->heap->elements[0];
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (!intern->heap->count || !*element) {
		RETURN_NULL();
	} else {
		zval **data = spl_pqueue_extract_helper(element, intern->flags);

		if (!data) {
			zend_error(E_RECOVERABLE_ERROR, "Unable to extract from the PriorityQueue node");
			RETURN_NULL();
		}

		RETURN_ZVAL(*data, 1, 0);
	}
}