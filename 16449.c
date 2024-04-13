static unsigned int __io_put_kbuf(struct io_kiocb *req, struct list_head *list)
{
	struct io_buffer *kbuf = req->kbuf;
	unsigned int cflags;

	cflags = IORING_CQE_F_BUFFER | (kbuf->bid << IORING_CQE_BUFFER_SHIFT);
	req->flags &= ~REQ_F_BUFFER_SELECTED;
	list_add(&kbuf->list, list);
	req->kbuf = NULL;
	return cflags;
}