PHP_MINIT_FUNCTION(spl_heap) /* {{{ */
{
	REGISTER_SPL_STD_CLASS_EX(SplHeap, spl_heap_object_new, spl_funcs_SplHeap);
	memcpy(&spl_handler_SplHeap, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

	spl_handler_SplHeap.clone_obj      = spl_heap_object_clone;
	spl_handler_SplHeap.count_elements = spl_heap_object_count_elements;
	spl_handler_SplHeap.get_debug_info = spl_heap_object_get_debug_info;

	REGISTER_SPL_IMPLEMENTS(SplHeap, Iterator);
	REGISTER_SPL_IMPLEMENTS(SplHeap, Countable);

	spl_ce_SplHeap->get_iterator = spl_heap_get_iterator;

	REGISTER_SPL_SUB_CLASS_EX(SplMinHeap,           SplHeap,        spl_heap_object_new, spl_funcs_SplMinHeap);
	REGISTER_SPL_SUB_CLASS_EX(SplMaxHeap,           SplHeap,        spl_heap_object_new, spl_funcs_SplMaxHeap);

	spl_ce_SplMaxHeap->get_iterator = spl_heap_get_iterator;
	spl_ce_SplMinHeap->get_iterator = spl_heap_get_iterator;

	REGISTER_SPL_STD_CLASS_EX(SplPriorityQueue, spl_heap_object_new, spl_funcs_SplPriorityQueue);
	memcpy(&spl_handler_SplPriorityQueue, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

	spl_handler_SplPriorityQueue.clone_obj      = spl_heap_object_clone;
	spl_handler_SplPriorityQueue.count_elements = spl_heap_object_count_elements;
	spl_handler_SplPriorityQueue.get_debug_info = spl_pqueue_object_get_debug_info;

	REGISTER_SPL_IMPLEMENTS(SplPriorityQueue, Iterator);
	REGISTER_SPL_IMPLEMENTS(SplPriorityQueue, Countable);

	spl_ce_SplPriorityQueue->get_iterator = spl_pqueue_get_iterator;

	REGISTER_SPL_CLASS_CONST_LONG(SplPriorityQueue, "EXTR_BOTH",      SPL_PQUEUE_EXTR_BOTH);
	REGISTER_SPL_CLASS_CONST_LONG(SplPriorityQueue, "EXTR_PRIORITY",  SPL_PQUEUE_EXTR_PRIORITY);
	REGISTER_SPL_CLASS_CONST_LONG(SplPriorityQueue, "EXTR_DATA",      SPL_PQUEUE_EXTR_DATA);

	return SUCCESS;
}