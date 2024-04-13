static void _php_curl_close(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	php_curl *ch = (php_curl *) rsrc->ptr;
	_php_curl_close_ex(ch TSRMLS_CC);
}