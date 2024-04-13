SPL_METHOD(SplPriorityQueue, insert)
{
	zval *data, *priority, *elem;
	spl_heap_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &data, &priority) == FAILURE) {
		return;
	}

	intern = (spl_heap_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (intern->heap->flags & SPL_HEAP_CORRUPTED) {
		zend_throw_exception(spl_ce_RuntimeException, "Heap is corrupted, heap properties are no longer ensured.", 0 TSRMLS_CC);
		return;
	}

	SEPARATE_ARG_IF_REF(data);
	SEPARATE_ARG_IF_REF(priority);

	ALLOC_INIT_ZVAL(elem);

	array_init(elem);
	add_assoc_zval_ex(elem, "data",     sizeof("data"),     data);
	add_assoc_zval_ex(elem, "priority", sizeof("priority"), priority);

	spl_ptr_heap_insert(intern->heap, elem, getThis() TSRMLS_CC);

	RETURN_TRUE;
} 