
static bool io_cancel_task_cb(struct io_wq_work *work, void *data)
{
	struct io_kiocb *req = container_of(work, struct io_kiocb, work);
	struct io_task_cancel *cancel = data;

	return io_match_task_safe(req, cancel->task, cancel->all);