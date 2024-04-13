static void io_req_task_queue_fail(struct io_kiocb *req, int ret)
{
	req->result = ret;
	req->io_task_work.func = io_req_task_cancel;
	io_req_task_work_add(req, false);
}