static inline void __io_req_complete(struct io_kiocb *req, unsigned issue_flags,
				     s32 res, u32 cflags)
{
	if (issue_flags & IO_URING_F_COMPLETE_DEFER)
		io_req_complete_state(req, res, cflags);
	else
		io_req_complete_post(req, res, cflags);
}