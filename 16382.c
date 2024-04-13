
static void io_poll_queue_proc(struct file *file, struct wait_queue_head *head,
			       struct poll_table_struct *p)
{
	struct io_poll_table *pt = container_of(p, struct io_poll_table, pt);

	__io_queue_proc(&pt->req->poll, pt, head,
			(struct io_poll_iocb **) &pt->req->async_data);