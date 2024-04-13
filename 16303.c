	__must_hold(&ctx->uring_lock)
{
	ctx->rsrc_cached_refs += IO_RSRC_REF_BATCH;
	percpu_ref_get_many(&ctx->rsrc_node->refs, IO_RSRC_REF_BATCH);
}