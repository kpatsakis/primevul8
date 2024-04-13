static void io_req_map_rw(struct io_kiocb *req, const struct iovec *iovec,
			  const struct iovec *fast_iov, struct iov_iter *iter)
{
	struct io_async_rw *rw = req->async_data;

	memcpy(&rw->s.iter, iter, sizeof(*iter));
	rw->free_iovec = iovec;
	rw->bytes_done = 0;
	/* can only be fixed buffers, no need to do anything */
	if (iov_iter_is_bvec(iter))
		return;
	if (!iovec) {
		unsigned iov_off = 0;

		rw->s.iter.iov = rw->s.fast_iov;
		if (iter->iov != fast_iov) {
			iov_off = iter->iov - fast_iov;
			rw->s.iter.iov += iov_off;
		}
		if (rw->s.fast_iov != fast_iov)
			memcpy(rw->s.fast_iov + iov_off, fast_iov + iov_off,
			       sizeof(struct iovec) * iter->nr_segs);
	} else {
		req->flags |= REQ_F_NEED_CLEANUP;
	}
}