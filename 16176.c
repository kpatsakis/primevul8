
static int io_poll_add(struct io_kiocb *req, unsigned int issue_flags)
{
	struct io_poll_iocb *poll = &req->poll;
	struct io_poll_table ipt;
	int ret;

	ipt.pt._qproc = io_poll_queue_proc;

	ret = __io_arm_poll_handler(req, &req->poll, &ipt, poll->events);
	ret = ret ?: ipt.error;
	if (ret)
		__io_req_complete(req, issue_flags, ret, 0);
	return 0;