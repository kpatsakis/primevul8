static void spl_heap_it_move_forward(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	zval                 *object   = (zval*)((zend_user_iterator *)iter)->it.data;
	spl_heap_it          *iterator = (spl_heap_it *)iter;
	spl_ptr_heap_element elem;

	if (iterator->object->heap->flags & SPL_HEAP_CORRUPTED) {
		zend_throw_exception(spl_ce_RuntimeException, "Heap is corrupted, heap properties are no longer ensured.", 0 TSRMLS_CC);
		return;
	}

	elem = spl_ptr_heap_delete_top(iterator->object->heap, object TSRMLS_CC);

	if (elem != NULL) {
		zval_ptr_dtor((zval **)&elem);
	}

	zend_user_it_invalidate_current(iter TSRMLS_CC);
}