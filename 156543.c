static spl_ptr_heap_element spl_ptr_heap_top(spl_ptr_heap *heap) { /* {{{ */
	if (heap->count == 0) {
		return NULL;
	}

	return heap->elements[0];
}