static inline void io_req_task_complete(struct io_kiocb *req, bool *locked)
{
	int res = req->result;

	if (*locked) {
		io_req_complete_state(req, res, io_put_kbuf(req, 0));
		io_req_add_compl_list(req);
	} else {
		io_req_complete_post(req, res,
					io_put_kbuf(req, IO_URING_F_UNLOCKED));
	}
}