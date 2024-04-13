 */
static void php_wddx_serialize_string(wddx_packet *packet, zval *var TSRMLS_DC)
{
	php_wddx_add_chunk_static(packet, WDDX_STRING_S);

	if (Z_STRLEN_P(var) > 0) {
		char *buf;
		size_t buf_len;

		buf = php_escape_html_entities(Z_STRVAL_P(var), Z_STRLEN_P(var), &buf_len, 0, ENT_QUOTES, NULL TSRMLS_CC);

		php_wddx_add_chunk_ex(packet, buf, buf_len);

		str_efree(buf);
	}
	php_wddx_add_chunk_static(packet, WDDX_STRING_E);