static HashTable* spl_pqueue_object_get_debug_info(zval *obj, int *is_temp TSRMLS_DC) /* {{{ */
{
	return spl_heap_object_get_debug_info_helper(spl_ce_SplPriorityQueue, obj, is_temp TSRMLS_CC);
}