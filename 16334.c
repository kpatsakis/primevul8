static void io_account_cq_overflow(struct io_ring_ctx *ctx)
{
	struct io_rings *r = ctx->rings;

	WRITE_ONCE(r->cq_overflow, READ_ONCE(r->cq_overflow) + 1);
	ctx->cq_extra--;
}