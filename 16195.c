static struct io_buffer *io_buffer_select(struct io_kiocb *req, size_t *len,
					  int bgid, unsigned int issue_flags)
{
	struct io_buffer *kbuf = req->kbuf;
	bool needs_lock = issue_flags & IO_URING_F_UNLOCKED;
	struct io_ring_ctx *ctx = req->ctx;
	struct io_buffer_list *bl;

	if (req->flags & REQ_F_BUFFER_SELECTED)
		return kbuf;

	io_ring_submit_lock(ctx, needs_lock);

	lockdep_assert_held(&ctx->uring_lock);

	bl = io_buffer_get_list(ctx, bgid);
	if (bl && !list_empty(&bl->buf_list)) {
		kbuf = list_first_entry(&bl->buf_list, struct io_buffer, list);
		list_del(&kbuf->list);
		if (*len > kbuf->len)
			*len = kbuf->len;
		req->flags |= REQ_F_BUFFER_SELECTED;
		req->kbuf = kbuf;
	} else {
		kbuf = ERR_PTR(-ENOBUFS);
	}

	io_ring_submit_unlock(req->ctx, needs_lock);
	return kbuf;
}