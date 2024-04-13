PHPAPI char *php_escape_html_entities(unsigned char *old, size_t oldlen, size_t *newlen, int all, int flags, char *hint_charset TSRMLS_DC)
{
	return php_escape_html_entities_ex(old, oldlen, newlen, all, flags, hint_charset, 1 TSRMLS_CC);
}