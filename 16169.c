
static void io_req_task_link_timeout(struct io_kiocb *req, bool *locked)
{
	struct io_kiocb *prev = req->timeout.prev;
	int ret = -ENOENT;

	if (prev) {
		if (!(req->task->flags & PF_EXITING))
			ret = io_try_cancel_userdata(req, prev->user_data);
		io_req_complete_post(req, ret ?: -ETIME, 0);
		io_put_req(prev);
	} else {
		io_req_complete_post(req, -ETIME, 0);
	}