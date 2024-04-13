
static int io_close_fixed(struct io_kiocb *req, unsigned int issue_flags)
{
	unsigned int offset = req->close.file_slot - 1;
	struct io_ring_ctx *ctx = req->ctx;
	bool needs_lock = issue_flags & IO_URING_F_UNLOCKED;
	struct io_fixed_file *file_slot;
	struct file *file;
	int ret;

	io_ring_submit_lock(ctx, needs_lock);
	ret = -ENXIO;
	if (unlikely(!ctx->file_data))
		goto out;
	ret = -EINVAL;
	if (offset >= ctx->nr_user_files)
		goto out;
	ret = io_rsrc_node_switch_start(ctx);
	if (ret)
		goto out;

	offset = array_index_nospec(offset, ctx->nr_user_files);
	file_slot = io_fixed_file_slot(&ctx->file_table, offset);
	ret = -EBADF;
	if (!file_slot->file_ptr)
		goto out;

	file = (struct file *)(file_slot->file_ptr & FFS_MASK);
	ret = io_queue_rsrc_removal(ctx->file_data, offset, ctx->rsrc_node, file);
	if (ret)
		goto out;

	file_slot->file_ptr = 0;
	io_rsrc_node_switch(ctx, ctx->file_data);
	ret = 0;
out:
	io_ring_submit_unlock(ctx, needs_lock);
	return ret;