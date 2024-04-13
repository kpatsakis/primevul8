void spl_array_iterator_key(zval *object, zval *return_value TSRMLS_DC) /* {{{ */
{
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	HashTable *aht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);

	if (spl_array_object_verify_pos(intern, aht TSRMLS_CC) == FAILURE) {
		return;
	}

	zend_hash_get_current_key_zval_ex(aht, return_value, &intern->pos);
}