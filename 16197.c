static inline struct io_kiocb *io_req_find_next(struct io_kiocb *req)
{
	struct io_kiocb *nxt;

	if (likely(!(req->flags & (REQ_F_LINK|REQ_F_HARDLINK))))
		return NULL;
	/*
	 * If LINK is set, we have dependent requests in this chain. If we
	 * didn't fail this request, queue the first one up, moving any other
	 * dependencies to the next request. In case of failure, fail the rest
	 * of the chain.
	 */
	if (unlikely(req->flags & IO_DISARM_MASK))
		__io_req_find_next_prep(req);
	nxt = req->link;
	req->link = NULL;
	return nxt;
}