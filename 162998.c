void zend_shutdown_extensions(TSRMLS_D)
{
	zend_llist_apply(&zend_extensions, (llist_apply_func_t) zend_extension_shutdown TSRMLS_CC);
	zend_llist_destroy(&zend_extensions);
}