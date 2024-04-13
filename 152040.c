static void *php_xml_malloc_wrapper(size_t sz)
{
	return emalloc(sz);
}