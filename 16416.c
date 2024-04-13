static inline struct io_kiocb *io_prep_linked_timeout(struct io_kiocb *req)
{
	if (likely(!(req->flags & REQ_F_ARM_LTIMEOUT)))
		return NULL;
	return __io_prep_linked_timeout(req);
}