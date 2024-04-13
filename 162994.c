int zend_register_extension(zend_extension *new_extension, DL_HANDLE handle)
{
#if ZEND_EXTENSIONS_SUPPORT
	zend_extension extension;

	extension = *new_extension;
	extension.handle = handle;

	zend_extension_dispatch_message(ZEND_EXTMSG_NEW_EXTENSION, &extension);

	zend_llist_add_element(&zend_extensions, &extension);

	/*fprintf(stderr, "Loaded %s, version %s\n", extension.name, extension.version);*/
#endif

	return SUCCESS;
}