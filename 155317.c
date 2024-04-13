 PHPAPI unsigned int php_next_utf8_char(
		const unsigned char *str,
		size_t str_len,
		size_t *cursor,
		int *status)
{
	return get_next_char(cs_utf_8, str, str_len, cursor, status);
}