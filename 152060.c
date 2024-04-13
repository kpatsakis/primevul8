static void php_filter_strip(zval *value, zend_long flags)
{
	unsigned char *str;
	int   i, c;
	zend_string *buf;

	/* Optimization for if no strip flags are set */
	if (!(flags & (FILTER_FLAG_STRIP_LOW | FILTER_FLAG_STRIP_HIGH | FILTER_FLAG_STRIP_BACKTICK))) {
		return;
	}

	str = (unsigned char *)Z_STRVAL_P(value);
	buf = zend_string_alloc(Z_STRLEN_P(value) + 1, 0);
	c = 0;
	for (i = 0; i < Z_STRLEN_P(value); i++) {
		if ((str[i] >= 127) && (flags & FILTER_FLAG_STRIP_HIGH)) {
		} else if ((str[i] < 32) && (flags & FILTER_FLAG_STRIP_LOW)) {
		} else if ((str[i] == '`') && (flags & FILTER_FLAG_STRIP_BACKTICK)) {
		} else {
			ZSTR_VAL(buf)[c] = str[i];
			++c;
		}
	}
	/* update zval string data */
	ZSTR_VAL(buf)[c] = '\0';
	ZSTR_LEN(buf) = c;
	zval_ptr_dtor(value);
	ZVAL_NEW_STR(value, buf);
}