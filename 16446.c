static inline bool io_req_ffs_set(struct io_kiocb *req)
{
	return req->flags & REQ_F_FIXED_FILE;
}