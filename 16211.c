static int io_setup_async_rw(struct io_kiocb *req, const struct iovec *iovec,
			     struct io_rw_state *s, bool force)
{
	if (!force && !io_op_defs[req->opcode].needs_async_setup)
		return 0;
	if (!req_has_async_data(req)) {
		struct io_async_rw *iorw;

		if (io_alloc_async_data(req)) {
			kfree(iovec);
			return -ENOMEM;
		}

		io_req_map_rw(req, iovec, s->fast_iov, &s->iter);
		iorw = req->async_data;
		/* we've copied and mapped the iter, ensure state is saved */
		iov_iter_save_state(&iorw->s.iter, &iorw->s.iter_state);
	}
	return 0;
}