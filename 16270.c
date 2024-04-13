static void io_req_complete_failed(struct io_kiocb *req, s32 res)
{
	req_set_fail(req);
	io_req_complete_post(req, res, io_put_kbuf(req, IO_URING_F_UNLOCKED));
}