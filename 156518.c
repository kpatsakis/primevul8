static void spl_ptr_heap_zval_dtor(spl_ptr_heap_element elem TSRMLS_DC) { /* {{{ */
	if (elem) {
		zval_ptr_dtor((zval **)&elem);
	}
}