static inline unsigned int io_put_kbuf_comp(struct io_kiocb *req)
{
	lockdep_assert_held(&req->ctx->completion_lock);

	if (likely(!(req->flags & REQ_F_BUFFER_SELECTED)))
		return 0;
	return __io_put_kbuf(req, &req->ctx->io_buffers_comp);
}