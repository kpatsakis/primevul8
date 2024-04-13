static int spl_heap_it_valid(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	spl_heap_it         *iterator = (spl_heap_it *)iter;

	return (iterator->object->heap->count != 0 ? SUCCESS : FAILURE);
}