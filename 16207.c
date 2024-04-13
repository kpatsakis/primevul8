static void io_complete_rw_iopoll(struct kiocb *kiocb, long res)
{
	struct io_kiocb *req = container_of(kiocb, struct io_kiocb, rw.kiocb);

	if (kiocb->ki_flags & IOCB_WRITE)
		kiocb_end_write(req);
	if (unlikely(res != req->result)) {
		if (res == -EAGAIN && io_rw_should_reissue(req)) {
			req->flags |= REQ_F_REISSUE;
			return;
		}
		req->result = res;
	}

	/* order with io_iopoll_complete() checking ->iopoll_completed */
	smp_store_release(&req->iopoll_completed, 1);
}