static void io_complete_rw(struct kiocb *kiocb, long res)
{
	struct io_kiocb *req = container_of(kiocb, struct io_kiocb, rw.kiocb);

	if (__io_complete_rw_common(req, res))
		return;
	req->result = res;
	req->io_task_work.func = io_req_task_complete;
	io_req_task_work_add(req, !!(req->ctx->flags & IORING_SETUP_SQPOLL));
}