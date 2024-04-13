static inline bool __io_fill_cqe_req(struct io_kiocb *req, s32 res, u32 cflags)
{
	trace_io_uring_complete(req->ctx, req, req->user_data, res, cflags);
	return __io_fill_cqe(req->ctx, req->user_data, res, cflags);
}