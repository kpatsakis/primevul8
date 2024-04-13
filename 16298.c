static void io_req_task_queue_reissue(struct io_kiocb *req)
{
	req->io_task_work.func = io_queue_async_work;
	io_req_task_work_add(req, false);
}