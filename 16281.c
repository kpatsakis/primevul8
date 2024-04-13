static inline bool io_alloc_async_data(struct io_kiocb *req)
{
	WARN_ON_ONCE(!io_op_defs[req->opcode].async_size);
	req->async_data = kmalloc(io_op_defs[req->opcode].async_size, GFP_KERNEL);
	if (req->async_data) {
		req->flags |= REQ_F_ASYNC_DATA;
		return false;
	}
	return true;
}