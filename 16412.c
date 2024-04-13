static inline bool io_file_supports_nowait(struct io_kiocb *req)
{
	return req->flags & REQ_F_SUPPORT_NOWAIT;
}