static void spl_filesystem_dir_it_dtor(zend_object_iterator *iter)
{
	spl_filesystem_iterator *iterator = (spl_filesystem_iterator *)iter;

	if (!Z_ISUNDEF(iterator->intern.data)) {
		zval *object = &iterator->intern.data;
		zval_ptr_dtor(object);
	}
	/* Otherwise we were called from the owning object free storage handler as
	 * it sets iterator->intern.data to IS_UNDEF.
	 * We don't even need to destroy iterator->current as we didn't add a
	 * reference to it in move_forward or get_iterator */
}