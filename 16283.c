static int io_splice(struct io_kiocb *req, unsigned int issue_flags)
{
	struct io_splice *sp = &req->splice;
	struct file *out = sp->file_out;
	unsigned int flags = sp->flags & ~SPLICE_F_FD_IN_FIXED;
	loff_t *poff_in, *poff_out;
	struct file *in;
	long ret = 0;

	if (issue_flags & IO_URING_F_NONBLOCK)
		return -EAGAIN;

	if (sp->flags & SPLICE_F_FD_IN_FIXED)
		in = io_file_get_fixed(req, sp->splice_fd_in, issue_flags);
	else
		in = io_file_get_normal(req, sp->splice_fd_in);
	if (!in) {
		ret = -EBADF;
		goto done;
	}

	poff_in = (sp->off_in == -1) ? NULL : &sp->off_in;
	poff_out = (sp->off_out == -1) ? NULL : &sp->off_out;

	if (sp->len)
		ret = do_splice(in, poff_in, out, poff_out, sp->len, flags);

	if (!(sp->flags & SPLICE_F_FD_IN_FIXED))
		io_put_file(in);
done:
	if (ret != sp->len)
		req_set_fail(req);
	io_req_complete(req, ret);
	return 0;
}