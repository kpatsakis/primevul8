static inline int io_import_iovec(int rw, struct io_kiocb *req,
				  struct iovec **iovec, struct io_rw_state *s,
				  unsigned int issue_flags)
{
	*iovec = __io_import_iovec(rw, req, s, issue_flags);
	if (unlikely(IS_ERR(*iovec)))
		return PTR_ERR(*iovec);

	iov_iter_save_state(&s->iter, &s->iter_state);
	return 0;
}