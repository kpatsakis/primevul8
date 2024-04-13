static size_t phar_dir_read(php_stream *stream, char *buf, size_t count TSRMLS_DC) /* {{{ */
{
	size_t to_read;
	HashTable *data = (HashTable *)stream->abstract;
	phar_zstr key;
	char *str_key;
	uint keylen;
	ulong unused;

	if (FAILURE == zend_hash_has_more_elements(data)) {
		return 0;
	}

	if (HASH_KEY_NON_EXISTENT == zend_hash_get_current_key_ex(data, &key, &keylen, &unused, 0, NULL)) {
		return 0;
	}

	PHAR_STR(key, str_key);
	zend_hash_move_forward(data);
	to_read = MIN(keylen, count);

	if (to_read == 0 || count < keylen) {
		PHAR_STR_FREE(str_key);
		return 0;
	}

	memset(buf, 0, sizeof(php_stream_dirent));
	memcpy(((php_stream_dirent *) buf)->d_name, str_key, to_read);
	PHAR_STR_FREE(str_key);
	((php_stream_dirent *) buf)->d_name[to_read + 1] = '\0';

	return sizeof(php_stream_dirent);
}