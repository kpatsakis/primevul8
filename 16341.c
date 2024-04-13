
static __cold int io_unregister_iowq_aff(struct io_ring_ctx *ctx)
{
	struct io_uring_task *tctx = current->io_uring;

	if (!tctx || !tctx->io_wq)
		return -EINVAL;

	return io_wq_cpu_affinity(tctx->io_wq, NULL);