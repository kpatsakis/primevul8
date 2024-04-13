PHP_FUNCTION(html_entity_decode)
{
	char *str, *hint_charset = NULL;
	int str_len, hint_charset_len = 0;
	size_t new_len = 0;
	long quote_style = ENT_COMPAT;
	char *replaced;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|ls", &str, &str_len,
							  &quote_style, &hint_charset, &hint_charset_len) == FAILURE) {
		return;
	}

	replaced = php_unescape_html_entities(str, str_len, &new_len, 1 /*all*/, quote_style, hint_charset TSRMLS_CC);
	if (replaced) {
		RETURN_STRINGL(replaced, (int)new_len, 0);
	}
	RETURN_FALSE;
}