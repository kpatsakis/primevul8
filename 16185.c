static inline struct io_kiocb *io_alloc_req(struct io_ring_ctx *ctx)
{
	struct io_wq_work_node *node;

	node = wq_stack_extract(&ctx->submit_state.free_list);
	return container_of(node, struct io_kiocb, comp_list);
}