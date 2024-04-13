static inline void io_req_put_rsrc(struct io_kiocb *req, struct io_ring_ctx *ctx)
{
	if (req->fixed_rsrc_refs)
		percpu_ref_put(req->fixed_rsrc_refs);
}