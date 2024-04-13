SPL_METHOD(SplHeap, key)
{
	spl_heap_object *intern = (spl_heap_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}		
	
	RETURN_LONG(intern->heap->count - 1);
}