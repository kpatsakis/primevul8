static void io_kbuf_recycle(struct io_kiocb *req, unsigned issue_flags)
{
	struct io_ring_ctx *ctx = req->ctx;
	struct io_buffer_list *bl;
	struct io_buffer *buf;

	if (likely(!(req->flags & REQ_F_BUFFER_SELECTED)))
		return;
	/* don't recycle if we already did IO to this buffer */
	if (req->flags & REQ_F_PARTIAL_IO)
		return;

	if (issue_flags & IO_URING_F_UNLOCKED)
		mutex_lock(&ctx->uring_lock);

	lockdep_assert_held(&ctx->uring_lock);

	buf = req->kbuf;
	bl = io_buffer_get_list(ctx, buf->bgid);
	list_add(&buf->list, &bl->buf_list);
	req->flags &= ~REQ_F_BUFFER_SELECTED;
	req->kbuf = NULL;

	if (issue_flags & IO_URING_F_UNLOCKED)
		mutex_unlock(&ctx->uring_lock);
}