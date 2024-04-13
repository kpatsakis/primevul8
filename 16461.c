
static inline struct file *io_file_get_fixed(struct io_kiocb *req, int fd,
					     unsigned int issue_flags)
{
	struct io_ring_ctx *ctx = req->ctx;
	struct file *file = NULL;
	unsigned long file_ptr;

	if (issue_flags & IO_URING_F_UNLOCKED)
		mutex_lock(&ctx->uring_lock);

	if (unlikely((unsigned int)fd >= ctx->nr_user_files))
		goto out;
	fd = array_index_nospec(fd, ctx->nr_user_files);
	file_ptr = io_fixed_file_slot(&ctx->file_table, fd)->file_ptr;
	file = (struct file *) (file_ptr & FFS_MASK);
	file_ptr &= ~FFS_MASK;
	/* mask in overlapping REQ_F and FFS bits */
	req->flags |= (file_ptr << REQ_F_SUPPORT_NOWAIT_BIT);
	io_req_set_rsrc_node(req, ctx, 0);
out:
	if (issue_flags & IO_URING_F_UNLOCKED)
		mutex_unlock(&ctx->uring_lock);
	return file;