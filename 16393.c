
static void *io_mem_alloc(size_t size)
{
	gfp_t gfp = GFP_KERNEL_ACCOUNT | __GFP_ZERO | __GFP_NOWARN | __GFP_COMP;

	return (void *) __get_free_pages(gfp, get_order(size));