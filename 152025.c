static void *php_xml_realloc_wrapper(void *ptr, size_t sz)
{
	return erealloc(ptr, sz);
}