static inline void io_dismantle_req(struct io_kiocb *req)
{
	unsigned int flags = req->flags;

	if (unlikely(flags & IO_REQ_CLEAN_FLAGS))
		io_clean_op(req);
	if (!(flags & REQ_F_FIXED_FILE))
		io_put_file(req->file);
}