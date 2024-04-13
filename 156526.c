static HashTable* spl_heap_object_get_debug_info_helper(zend_class_entry *ce, zval *obj, int *is_temp TSRMLS_DC) { /* {{{ */
	spl_heap_object *intern  = (spl_heap_object*)zend_object_store_get_object(obj TSRMLS_CC);
	zval *tmp, zrv, *heap_array;
	char *pnstr;
	int  pnlen;
	int  i;

	*is_temp = 0;

	if (!intern->std.properties) {
		rebuild_object_properties(&intern->std);
	}

	if (intern->debug_info == NULL) {
		ALLOC_HASHTABLE(intern->debug_info);
		ZEND_INIT_SYMTABLE_EX(intern->debug_info, zend_hash_num_elements(intern->std.properties) + 1, 0);
	}

	if (intern->debug_info->nApplyCount == 0) {
		INIT_PZVAL(&zrv);
		Z_ARRVAL(zrv) = intern->debug_info;

		zend_hash_copy(intern->debug_info, intern->std.properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

		pnstr = spl_gen_private_prop_name(ce, "flags", sizeof("flags")-1, &pnlen TSRMLS_CC);
		add_assoc_long_ex(&zrv, pnstr, pnlen+1, intern->flags);
		efree(pnstr);

		pnstr = spl_gen_private_prop_name(ce, "isCorrupted", sizeof("isCorrupted")-1, &pnlen TSRMLS_CC);
		add_assoc_bool_ex(&zrv, pnstr, pnlen+1, intern->heap->flags&SPL_HEAP_CORRUPTED);
		efree(pnstr);

		ALLOC_INIT_ZVAL(heap_array);
		array_init(heap_array);

		for (i = 0; i < intern->heap->count; ++i) {
			add_index_zval(heap_array, i, (zval *)intern->heap->elements[i]);
			Z_ADDREF_P(intern->heap->elements[i]);
		}

		pnstr = spl_gen_private_prop_name(ce, "heap", sizeof("heap")-1, &pnlen TSRMLS_CC);
		add_assoc_zval_ex(&zrv, pnstr, pnlen+1, heap_array);
		efree(pnstr);
	}

	return intern->debug_info;
}