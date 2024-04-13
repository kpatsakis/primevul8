static inline loff_t *io_kiocb_update_pos(struct io_kiocb *req)
{
	struct kiocb *kiocb = &req->rw.kiocb;

	if (kiocb->ki_pos != -1)
		return &kiocb->ki_pos;

	if (!(req->file->f_mode & FMODE_STREAM)) {
		req->flags |= REQ_F_CUR_POS;
		kiocb->ki_pos = req->file->f_pos;
		return &kiocb->ki_pos;
	}

	kiocb->ki_pos = 0;
	return NULL;
}