static void io_task_refs_refill(struct io_uring_task *tctx)
{
	unsigned int refill = -tctx->cached_refs + IO_TCTX_REFS_CACHE_NR;

	percpu_counter_add(&tctx->inflight, refill);
	refcount_add(refill, &current->usage);
	tctx->cached_refs += refill;
}