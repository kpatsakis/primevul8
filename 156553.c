SPL_METHOD(SplPriorityQueue, setExtractFlags)
{
	long value;
	spl_heap_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &value) == FAILURE) {
		return;
	}

	intern = (spl_heap_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	intern->flags = value & SPL_PQUEUE_EXTR_MASK;

	RETURN_LONG(intern->flags);
}