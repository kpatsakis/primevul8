static void php_xml_free_wrapper(void *ptr)
{
	if (ptr != NULL) {
		efree(ptr);
	}
}