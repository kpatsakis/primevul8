zend_object_iterator *spl_heap_get_iterator(zend_class_entry *ce, zval *object, int by_ref TSRMLS_DC) /* {{{ */
{
	spl_heap_it     *iterator;
	spl_heap_object *heap_object = (spl_heap_object*)zend_object_store_get_object(object TSRMLS_CC);

	if (by_ref) {
		zend_throw_exception(spl_ce_RuntimeException, "An iterator cannot be used with foreach by reference", 0 TSRMLS_CC);
		return NULL;
	}

	Z_ADDREF_P(object);

	iterator                  = emalloc(sizeof(spl_heap_it));
	iterator->intern.it.data  = (void*)object;
	iterator->intern.it.funcs = &spl_heap_it_funcs;
	iterator->intern.ce       = ce;
	iterator->intern.value    = NULL;
	iterator->flags           = heap_object->flags;
	iterator->object          = heap_object;

	return (zend_object_iterator*)iterator;
}