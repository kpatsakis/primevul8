static bool need_read_all(struct io_kiocb *req)
{
	return req->flags & REQ_F_ISREG ||
		S_ISBLK(file_inode(req->file)->i_mode);
}