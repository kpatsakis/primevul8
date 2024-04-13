static zend_object_value spl_heap_object_new_ex(zend_class_entry *class_type, spl_heap_object **obj, zval *orig, int clone_orig TSRMLS_DC) /* {{{ */
{
	zend_object_value  retval;
	spl_heap_object   *intern;
	zend_class_entry  *parent = class_type;
	int                inherited = 0;

	intern = ecalloc(1, sizeof(spl_heap_object));
	*obj = intern;
	ALLOC_INIT_ZVAL(intern->retval);

	zend_object_std_init(&intern->std, class_type TSRMLS_CC);
	object_properties_init(&intern->std, class_type);

	intern->flags      = 0;
	intern->fptr_cmp   = NULL;
	intern->debug_info = NULL;

	if (orig) {
		spl_heap_object *other = (spl_heap_object*)zend_object_store_get_object(orig TSRMLS_CC);
		intern->ce_get_iterator = other->ce_get_iterator;

		if (clone_orig) {
			int i;
			intern->heap = spl_ptr_heap_clone(other->heap TSRMLS_CC);
			for (i = 0; i < intern->heap->count; ++i) {
				if (intern->heap->elements[i]) {
					Z_ADDREF_P((zval *)intern->heap->elements[i]);
				}
			}
		} else {
			intern->heap = other->heap;
		}

		intern->flags = other->flags;
	} else {
		intern->heap = spl_ptr_heap_init(spl_ptr_heap_zval_max_cmp, spl_ptr_heap_zval_ctor, spl_ptr_heap_zval_dtor);
	}

	retval.handlers = &spl_handler_SplHeap;

	while (parent) {
		if (parent == spl_ce_SplPriorityQueue) {
			intern->heap->cmp = spl_ptr_pqueue_zval_cmp;
			intern->flags     = SPL_PQUEUE_EXTR_DATA;
			retval.handlers   = &spl_handler_SplPriorityQueue;
			break;
		}

		if (parent == spl_ce_SplMinHeap) {
			intern->heap->cmp = spl_ptr_heap_zval_min_cmp;
			break;
		}

		if (parent == spl_ce_SplMaxHeap) {
			intern->heap->cmp = spl_ptr_heap_zval_max_cmp;
			break;
		}

		if (parent == spl_ce_SplHeap) {
			break;
		}

		parent = parent->parent;
		inherited = 1;
	}

	retval.handle   = zend_objects_store_put(intern, (zend_objects_store_dtor_t)zend_objects_destroy_object, spl_heap_object_free_storage, NULL TSRMLS_CC);

	if (!parent) { /* this must never happen */
		php_error_docref(NULL TSRMLS_CC, E_COMPILE_ERROR, "Internal compiler error, Class is not child of SplHeap");
	}

	if (inherited) {
		zend_hash_find(&class_type->function_table, "compare",    sizeof("compare"),    (void **) &intern->fptr_cmp);
		if (intern->fptr_cmp->common.scope == parent) {
			intern->fptr_cmp = NULL;
		}
		zend_hash_find(&class_type->function_table, "count",        sizeof("count"),        (void **) &intern->fptr_count);
		if (intern->fptr_count->common.scope == parent) {
			intern->fptr_count = NULL;
		}
	}

	return retval;
}