 */
static inline bool io_check_restriction(struct io_ring_ctx *ctx,
					struct io_kiocb *req,
					unsigned int sqe_flags)
{
	if (!test_bit(req->opcode, ctx->restrictions.sqe_op))
		return false;

	if ((sqe_flags & ctx->restrictions.sqe_flags_required) !=
	    ctx->restrictions.sqe_flags_required)
		return false;

	if (sqe_flags & ~(ctx->restrictions.sqe_flags_allowed |
			  ctx->restrictions.sqe_flags_required))
		return false;

	return true;