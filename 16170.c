
static struct io_wq_work *io_wq_free_work(struct io_wq_work *work)
{
	struct io_kiocb *req = container_of(work, struct io_kiocb, work);

	req = io_put_req_find_next(req);
	return req ? &req->work : NULL;