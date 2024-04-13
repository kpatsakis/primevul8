static void spl_ptr_heap_zval_ctor(spl_ptr_heap_element elem TSRMLS_DC) { /* {{{ */
	Z_ADDREF_P((zval *)elem);
}