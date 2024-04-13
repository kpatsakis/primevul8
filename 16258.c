
static bool io_assign_file(struct io_kiocb *req, unsigned int issue_flags)
{
	if (req->file || !io_op_defs[req->opcode].needs_file)
		return true;

	if (req->flags & REQ_F_FIXED_FILE)
		req->file = io_file_get_fixed(req, req->fd, issue_flags);
	else
		req->file = io_file_get_normal(req, req->fd);
	if (req->file)
		return true;

	req_set_fail(req);
	req->result = -EBADF;
	return false;