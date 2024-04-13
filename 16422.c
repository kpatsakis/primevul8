static inline void io_req_complete(struct io_kiocb *req, s32 res)
{
	__io_req_complete(req, 0, res, 0);
}