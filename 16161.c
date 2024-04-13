static inline void io_put_req_deferred(struct io_kiocb *req)
{
	if (req_ref_put_and_test(req)) {
		req->io_task_work.func = io_free_req_work;
		io_req_task_work_add(req, false);
	}
}