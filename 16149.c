 */
static int io_ringfd_register(struct io_ring_ctx *ctx, void __user *__arg,
			      unsigned nr_args)
{
	struct io_uring_rsrc_update __user *arg = __arg;
	struct io_uring_rsrc_update reg;
	struct io_uring_task *tctx;
	int ret, i;

	if (!nr_args || nr_args > IO_RINGFD_REG_MAX)
		return -EINVAL;

	mutex_unlock(&ctx->uring_lock);
	ret = io_uring_add_tctx_node(ctx);
	mutex_lock(&ctx->uring_lock);
	if (ret)
		return ret;

	tctx = current->io_uring;
	for (i = 0; i < nr_args; i++) {
		int start, end;

		if (copy_from_user(&reg, &arg[i], sizeof(reg))) {
			ret = -EFAULT;
			break;
		}

		if (reg.resv) {
			ret = -EINVAL;
			break;
		}

		if (reg.offset == -1U) {
			start = 0;
			end = IO_RINGFD_REG_MAX;
		} else {
			if (reg.offset >= IO_RINGFD_REG_MAX) {
				ret = -EINVAL;
				break;
			}
			start = reg.offset;
			end = start + 1;
		}

		ret = io_ring_add_registered_fd(tctx, reg.data, start, end);
		if (ret < 0)
			break;

		reg.offset = ret;
		if (copy_to_user(&arg[i], &reg, sizeof(reg))) {
			fput(tctx->registered_rings[reg.offset]);
			tctx->registered_rings[reg.offset] = NULL;
			ret = -EFAULT;
			break;
		}
	}

	return i ? i : ret;