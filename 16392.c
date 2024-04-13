static inline void io_req_set_refcount(struct io_kiocb *req)
{
	__io_req_set_refcount(req, 1);
}