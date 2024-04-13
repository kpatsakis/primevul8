PHP_XML_API char *_xml_zval_strdup(zval *val)
{
	if (Z_TYPE_P(val) == IS_STRING) {
		char *buf = emalloc(Z_STRLEN_P(val) + 1);
		memcpy(buf, Z_STRVAL_P(val), Z_STRLEN_P(val));
		buf[Z_STRLEN_P(val)] = '\0';
		return buf;
	}
	return NULL;
}