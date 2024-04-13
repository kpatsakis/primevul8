static int io_msg_ring(struct io_kiocb *req, unsigned int issue_flags)
{
	struct io_ring_ctx *target_ctx;
	struct io_msg *msg = &req->msg;
	bool filled;
	int ret;

	ret = -EBADFD;
	if (req->file->f_op != &io_uring_fops)
		goto done;

	ret = -EOVERFLOW;
	target_ctx = req->file->private_data;

	spin_lock(&target_ctx->completion_lock);
	filled = io_fill_cqe_aux(target_ctx, msg->user_data, msg->len, 0);
	io_commit_cqring(target_ctx);
	spin_unlock(&target_ctx->completion_lock);

	if (filled) {
		io_cqring_ev_posted(target_ctx);
		ret = 0;
	}

done:
	if (ret < 0)
		req_set_fail(req);
	__io_req_complete(req, issue_flags, ret, 0);
	return 0;
}