static PHP_GINIT_FUNCTION(xml)
{
#if defined(COMPILE_DL_XML) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	xml_globals->default_encoding = (XML_Char*)"UTF-8";
}