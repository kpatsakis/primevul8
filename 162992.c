int zend_startup_extensions()
{
	zend_llist_apply_with_del(&zend_extensions, (int (*)(void *)) zend_extension_startup);
	return SUCCESS;
}