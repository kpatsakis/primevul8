static inline bool req_has_async_data(struct io_kiocb *req)
{
	return req->flags & REQ_F_ASYNC_DATA;
}