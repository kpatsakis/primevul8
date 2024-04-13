static void zend_extension_shutdown(zend_extension *extension TSRMLS_DC)
{
#if ZEND_EXTENSIONS_SUPPORT
	if (extension->shutdown) {
		extension->shutdown(extension);
	}
#endif
}