static void filter_map_apply(zval *value, filter_map *map)
{
	unsigned char *str;
	int   i, c;
	zend_string *buf;

	str = (unsigned char *)Z_STRVAL_P(value);
	buf = zend_string_alloc(Z_STRLEN_P(value) + 1, 0);
	c = 0;
	for (i = 0; i < Z_STRLEN_P(value); i++) {
		if ((*map)[str[i]]) {
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