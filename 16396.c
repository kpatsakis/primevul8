static struct io_kiocb *__io_prep_linked_timeout(struct io_kiocb *req)
{
	if (WARN_ON_ONCE(!req->link))
		return NULL;

	req->flags &= ~REQ_F_ARM_LTIMEOUT;
	req->flags |= REQ_F_LINK_TIMEOUT;

	/* linked timeouts should have two refs once prep'ed */
	io_req_set_refcount(req);
	__io_req_set_refcount(req->link, 2);
	return req->link;
}