static int phar_dir_close(php_stream *stream, int close_handle TSRMLS_DC)  /* {{{ */
{
	HashTable *data = (HashTable *)stream->abstract;

	if (data && data->arBuckets) {
		zend_hash_destroy(data);
		data->arBuckets = 0;
		FREE_HASHTABLE(data);
		stream->abstract = NULL;
	}

	return 0;
}