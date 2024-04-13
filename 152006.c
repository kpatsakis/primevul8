void php_filter_full_special_chars(PHP_INPUT_FILTER_PARAM_DECL)
{
	zend_string *buf;
	int quotes;

	if (!(flags & FILTER_FLAG_NO_ENCODE_QUOTES)) {
		quotes = ENT_QUOTES;
	} else {
		quotes = ENT_NOQUOTES;
	}
	buf = php_escape_html_entities_ex((unsigned char *) Z_STRVAL_P(value), Z_STRLEN_P(value), 1, quotes, SG(default_charset), 0);
	zval_ptr_dtor(value);
	ZVAL_STR(value, buf);
}