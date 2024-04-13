static void spl_array_it_get_current_key(zend_object_iterator *iter, zval *key TSRMLS_DC) /* {{{ */
{
	spl_array_it       *iterator = (spl_array_it *)iter;
	spl_array_object   *object   = iterator->object;
	HashTable          *aht      = spl_array_get_hash_table(object, 0 TSRMLS_CC);

	if (object->ar_flags & SPL_ARRAY_OVERLOADED_KEY) {
		zend_user_it_get_current_key(iter, key TSRMLS_CC);
	} else {
		if (spl_array_object_verify_pos_ex(object, aht, "ArrayIterator::current(): " TSRMLS_CC) == FAILURE) {
			ZVAL_NULL(key);
		} else {
			zend_hash_get_current_key_zval_ex(aht, key, &object->pos);
		}
	}
}