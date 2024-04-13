ZEND_API void zend_extension_dispatch_message(int message, void *arg)
{
	TSRMLS_FETCH();

	zend_llist_apply_with_arguments(&zend_extensions, (llist_apply_with_args_func_t) zend_extension_message_dispatcher TSRMLS_CC, 2, message, arg);
}