static void spl_heap_it_get_current_key(zend_object_iterator *iter, zval *key TSRMLS_DC) /* {{{ */
{
	spl_heap_it *iterator = (spl_heap_it *)iter;

	ZVAL_LONG(key, iterator->object->heap->count - 1);
}