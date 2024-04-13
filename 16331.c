
static int io_sqe_buffers_register(struct io_ring_ctx *ctx, void __user *arg,
				   unsigned int nr_args, u64 __user *tags)
{
	struct page *last_hpage = NULL;
	struct io_rsrc_data *data;
	int i, ret;
	struct iovec iov;

	if (ctx->user_bufs)
		return -EBUSY;
	if (!nr_args || nr_args > IORING_MAX_REG_BUFFERS)
		return -EINVAL;
	ret = io_rsrc_node_switch_start(ctx);
	if (ret)
		return ret;
	ret = io_rsrc_data_alloc(ctx, io_rsrc_buf_put, tags, nr_args, &data);
	if (ret)
		return ret;
	ret = io_buffers_map_alloc(ctx, nr_args);
	if (ret) {
		io_rsrc_data_free(data);
		return ret;
	}

	for (i = 0; i < nr_args; i++, ctx->nr_user_bufs++) {
		ret = io_copy_iov(ctx, &iov, arg, i);
		if (ret)
			break;
		ret = io_buffer_validate(&iov);
		if (ret)
			break;
		if (!iov.iov_base && *io_get_tag_slot(data, i)) {
			ret = -EINVAL;
			break;
		}

		ret = io_sqe_buffer_register(ctx, &iov, &ctx->user_bufs[i],
					     &last_hpage);
		if (ret)
			break;
	}

	WARN_ON_ONCE(ctx->buf_data);

	ctx->buf_data = data;
	if (ret)
		__io_sqe_buffers_unregister(ctx);
	else
		io_rsrc_node_switch(ctx, NULL);
	return ret;