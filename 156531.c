static spl_ptr_heap_element spl_ptr_heap_delete_top(spl_ptr_heap *heap, void *cmp_userdata TSRMLS_DC) { /* {{{ */
	int i, j;
	const int limit = (heap->count-1)/2;
	spl_ptr_heap_element top;
	spl_ptr_heap_element bottom;

	if (heap->count == 0) {
		return NULL;
	}

	top    = heap->elements[0];
	bottom = heap->elements[--heap->count];

	for( i = 0; i < limit; i = j)
	{
		/* Find smaller child */
		j = i*2+1;
		if(j != heap->count && heap->cmp(heap->elements[j+1], heap->elements[j], cmp_userdata TSRMLS_CC) > 0) {
			j++; /* next child is bigger */
		}

		/* swap elements between two levels */
		if(heap->cmp(bottom, heap->elements[j], cmp_userdata TSRMLS_CC) < 0) {
			heap->elements[i] = heap->elements[j];
		} else {
			break;
		}
	}

	if (EG(exception)) {
		/* exception thrown during comparison */
		heap->flags |= SPL_HEAP_CORRUPTED;
	}

	heap->elements[i] = bottom;
	heap->dtor(top TSRMLS_CC);
	return top;
}