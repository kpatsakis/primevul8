 */
void php_wddx_packet_start(wddx_packet *packet, char *comment, int comment_len)
{
	php_wddx_add_chunk_static(packet, WDDX_PACKET_S);
	if (comment) {
		char *escaped;
		size_t escaped_len;
		TSRMLS_FETCH();
		escaped = php_escape_html_entities(
			comment, comment_len, &escaped_len, 0, ENT_QUOTES, NULL TSRMLS_CC);

		php_wddx_add_chunk_static(packet, WDDX_HEADER_S);
		php_wddx_add_chunk_static(packet, WDDX_COMMENT_S);
		php_wddx_add_chunk_ex(packet, escaped, escaped_len);
		php_wddx_add_chunk_static(packet, WDDX_COMMENT_E);
		php_wddx_add_chunk_static(packet, WDDX_HEADER_E);

		str_efree(escaped);
	} else {
		php_wddx_add_chunk_static(packet, WDDX_HEADER);
	}
	php_wddx_add_chunk_static(packet, WDDX_DATA_S);