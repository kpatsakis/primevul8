static inline void io_req_add_compl_list(struct io_kiocb *req)
{
	struct io_ring_ctx *ctx = req->ctx;
	struct io_submit_state *state = &ctx->submit_state;

	if (!(req->flags & REQ_F_CQE_SKIP))
		ctx->submit_state.flush_cqes = true;
	wq_list_add_tail(&req->comp_list, &state->compl_reqs);
}