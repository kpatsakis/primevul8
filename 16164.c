static struct io_buffer *io_recv_buffer_select(struct io_kiocb *req,
					       unsigned int issue_flags)
{
	struct io_sr_msg *sr = &req->sr_msg;

	return io_buffer_select(req, &sr->len, sr->bgid, issue_flags);
}