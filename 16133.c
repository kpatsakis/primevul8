
static int io_install_fixed_file(struct io_kiocb *req, struct file *file,
				 unsigned int issue_flags, u32 slot_index)
{
	struct io_ring_ctx *ctx = req->ctx;
	bool needs_lock = issue_flags & IO_URING_F_UNLOCKED;
	bool needs_switch = false;
	struct io_fixed_file *file_slot;
	int ret = -EBADF;

	io_ring_submit_lock(ctx, needs_lock);
	if (file->f_op == &io_uring_fops)
		goto err;
	ret = -ENXIO;
	if (!ctx->file_data)
		goto err;
	ret = -EINVAL;
	if (slot_index >= ctx->nr_user_files)
		goto err;

	slot_index = array_index_nospec(slot_index, ctx->nr_user_files);
	file_slot = io_fixed_file_slot(&ctx->file_table, slot_index);

	if (file_slot->file_ptr) {
		struct file *old_file;

		ret = io_rsrc_node_switch_start(ctx);
		if (ret)
			goto err;

		old_file = (struct file *)(file_slot->file_ptr & FFS_MASK);
		ret = io_queue_rsrc_removal(ctx->file_data, slot_index,
					    ctx->rsrc_node, old_file);
		if (ret)
			goto err;
		file_slot->file_ptr = 0;
		needs_switch = true;
	}

	*io_get_tag_slot(ctx->file_data, slot_index) = 0;
	io_fixed_file_set(file_slot, file);
	ret = io_sqe_file_register(ctx, file, slot_index);
	if (ret) {
		file_slot->file_ptr = 0;
		goto err;
	}

	ret = 0;
err:
	if (needs_switch)
		io_rsrc_node_switch(ctx, ctx->file_data);
	io_ring_submit_unlock(ctx, needs_lock);
	if (ret)
		fput(file);
	return ret;