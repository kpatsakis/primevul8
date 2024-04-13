 */
static inline bool io_poll_get_ownership(struct io_kiocb *req)
{
	return !(atomic_fetch_inc(&req->poll_refs) & IO_POLL_REF_MASK);