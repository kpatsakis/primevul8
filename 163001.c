ZEND_API zend_extension *zend_get_extension(const char *extension_name)
{
	zend_llist_element *element;

	for (element = zend_extensions.head; element; element = element->next) {
		zend_extension *extension = (zend_extension *) element->data;

		if (!strcmp(extension->name, extension_name)) {
			return extension;
		}
	}
	return NULL;
}