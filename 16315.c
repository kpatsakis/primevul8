
static int io_ringfd_unregister(struct io_ring_ctx *ctx, void __user *__arg,
				unsigned nr_args)
{
	struct io_uring_rsrc_update __user *arg = __arg;
	struct io_uring_task *tctx = current->io_uring;
	struct io_uring_rsrc_update reg;
	int ret = 0, i;

	if (!nr_args || nr_args > IO_RINGFD_REG_MAX)
		return -EINVAL;
	if (!tctx)
		return 0;

	for (i = 0; i < nr_args; i++) {
		if (copy_from_user(&reg, &arg[i], sizeof(reg))) {
			ret = -EFAULT;
			break;
		}
		if (reg.resv || reg.offset >= IO_RINGFD_REG_MAX) {
			ret = -EINVAL;
			break;
		}

		reg.offset = array_index_nospec(reg.offset, IO_RINGFD_REG_MAX);
		if (tctx->registered_rings[reg.offset]) {
			fput(tctx->registered_rings[reg.offset]);
			tctx->registered_rings[reg.offset] = NULL;
		}
	}

	return i ? i : ret;