static void spl_ptr_heap_destroy(spl_ptr_heap *heap TSRMLS_DC) { /* {{{ */
	int i;

	for (i=0; i < heap->count; ++i) {
		heap->dtor(heap->elements[i] TSRMLS_CC);
	}

	efree(heap->elements);
	efree(heap);
}