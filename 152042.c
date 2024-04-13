PHP_XML_API zend_string *xml_utf8_decode(const XML_Char *s, size_t len, const XML_Char *encoding)
{
	size_t pos = 0;
	unsigned int c;
	char (*decoder)(unsigned short) = NULL;
	xml_encoding *enc = xml_get_encoding(encoding);
	zend_string *str;

	if (enc) {
		decoder = enc->decoding_function;
	}

	if (decoder == NULL) {
		/* If the target encoding was unknown, or no decoder function
		 * was specified, return the UTF-8-encoded data as-is.
		 */
		str = zend_string_init((char *)s, len, 0);
		return str;
	}

	str = zend_string_alloc(len, 0);
	ZSTR_LEN(str) = 0;
	while (pos < len) {
		int status = FAILURE;
		c = php_next_utf8_char((const unsigned char*)s, (size_t) len, &pos, &status);

		if (status == FAILURE || c > 0xFFU) {
			c = '?';
		}

		ZSTR_VAL(str)[ZSTR_LEN(str)++] = decoder ? decoder(c) : c;
	}
	ZSTR_VAL(str)[ZSTR_LEN(str)] = '\0';
	if (ZSTR_LEN(str) < len) {
		str = zend_string_truncate(str, ZSTR_LEN(str), 0);
	}

	return str;
}