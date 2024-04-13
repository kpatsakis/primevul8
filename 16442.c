
void io_uring_unreg_ringfd(void)
{
	struct io_uring_task *tctx = current->io_uring;
	int i;

	for (i = 0; i < IO_RINGFD_REG_MAX; i++) {
		if (tctx->registered_rings[i]) {
			fput(tctx->registered_rings[i]);
			tctx->registered_rings[i] = NULL;
		}
	}