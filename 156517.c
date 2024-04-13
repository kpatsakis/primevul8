static zval **spl_pqueue_extract_helper(zval **value, int flags) /* {{{ */
{
	if ((flags & SPL_PQUEUE_EXTR_BOTH) == SPL_PQUEUE_EXTR_BOTH) {
		return value;
	} else if ((flags & SPL_PQUEUE_EXTR_BOTH) > 0) {

		if ((flags & SPL_PQUEUE_EXTR_DATA) == SPL_PQUEUE_EXTR_DATA) {
			zval **data;
			if (zend_hash_find(Z_ARRVAL_PP(value), "data", sizeof("data"), (void **) &data) == SUCCESS) {
				return data;
			}
		} else {
			zval **priority;
			if (zend_hash_find(Z_ARRVAL_PP(value), "priority", sizeof("priority"), (void **) &priority) == SUCCESS) {
				return priority;
			}
		}
	}

	return NULL;
}