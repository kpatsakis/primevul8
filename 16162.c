static void __user *io_rw_buffer_select(struct io_kiocb *req, size_t *len,
					unsigned int issue_flags)
{
	struct io_buffer *kbuf;
	u16 bgid;

	bgid = req->buf_index;
	kbuf = io_buffer_select(req, len, bgid, issue_flags);
	if (IS_ERR(kbuf))
		return kbuf;
	return u64_to_user_ptr(kbuf->addr);
}