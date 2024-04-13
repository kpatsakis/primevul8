static int phar_dir_seek(php_stream *stream, off_t offset, int whence, off_t *newoffset TSRMLS_DC) /* {{{ */
{
	HashTable *data = (HashTable *)stream->abstract;

	if (!data) {
		return -1;
	}

	if (whence == SEEK_END) {
		whence = SEEK_SET;
		offset = zend_hash_num_elements(data) + offset;
	}

	if (whence == SEEK_SET) {
		zend_hash_internal_pointer_reset(data);
	}

	if (offset < 0) {
		return -1;
	} else {
		*newoffset = 0;
		while (*newoffset < offset && zend_hash_move_forward(data) == SUCCESS) {
			++(*newoffset);
		}
		return 0;
	}
}