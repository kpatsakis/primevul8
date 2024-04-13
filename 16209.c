static noinline void io_fill_cqe_req(struct io_kiocb *req, s32 res, u32 cflags)
{
	if (!(req->flags & REQ_F_CQE_SKIP))
		__io_fill_cqe_req(req, res, cflags);
}