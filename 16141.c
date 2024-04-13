static void kiocb_done(struct io_kiocb *req, ssize_t ret,
		       unsigned int issue_flags)
{
	struct io_async_rw *io = req->async_data;

	/* add previously done IO, if any */
	if (req_has_async_data(req) && io->bytes_done > 0) {
		if (ret < 0)
			ret = io->bytes_done;
		else
			ret += io->bytes_done;
	}

	if (req->flags & REQ_F_CUR_POS)
		req->file->f_pos = req->rw.kiocb.ki_pos;
	if (ret >= 0 && (req->rw.kiocb.ki_complete == io_complete_rw))
		__io_complete_rw(req, ret, issue_flags);
	else
		io_rw_done(&req->rw.kiocb, ret);

	if (req->flags & REQ_F_REISSUE) {
		req->flags &= ~REQ_F_REISSUE;
		if (io_resubmit_prep(req))
			io_req_task_queue_reissue(req);
		else
			io_req_task_queue_fail(req, ret);
	}
}