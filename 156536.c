static void spl_heap_object_free_storage(void *object TSRMLS_DC) /* {{{ */
{
	int i;
	spl_heap_object *intern = (spl_heap_object *)object;

	zend_object_std_dtor(&intern->std TSRMLS_CC);

	for (i = 0; i < intern->heap->count; ++i) {
		if (intern->heap->elements[i]) {
			zval_ptr_dtor((zval **)&intern->heap->elements[i]);
		}
	}

	spl_ptr_heap_destroy(intern->heap TSRMLS_CC);

	zval_ptr_dtor(&intern->retval);

	if (intern->debug_info != NULL) {
		zend_hash_destroy(intern->debug_info);
		efree(intern->debug_info);
	}

	efree(object);
}