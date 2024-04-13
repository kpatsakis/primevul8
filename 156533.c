static spl_ptr_heap *spl_ptr_heap_init(spl_ptr_heap_cmp_func cmp, spl_ptr_heap_ctor_func ctor, spl_ptr_heap_dtor_func dtor) /* {{{ */
{
	spl_ptr_heap *heap = emalloc(sizeof(spl_ptr_heap));

	heap->dtor     = dtor;
	heap->ctor     = ctor;
	heap->cmp      = cmp;
	heap->elements = safe_emalloc(sizeof(spl_ptr_heap_element), PTR_HEAP_BLOCK_SIZE, 0);
	heap->max_size = PTR_HEAP_BLOCK_SIZE;
	heap->count    = 0;
	heap->flags    = 0;

	return heap;
}