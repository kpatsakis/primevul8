	__must_hold(&ctx->uring_lock)
{
	struct percpu_ref *ref = req->fixed_rsrc_refs;

	if (ref) {
		if (ref == &ctx->rsrc_node->refs)
			ctx->rsrc_cached_refs++;
		else
			percpu_ref_put(ref);
	}
}