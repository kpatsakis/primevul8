	__must_hold(&ctx->uring_lock)
{
	if (ctx->rsrc_cached_refs) {
		percpu_ref_put_many(&ctx->rsrc_node->refs, ctx->rsrc_cached_refs);
		ctx->rsrc_cached_refs = 0;
	}
}