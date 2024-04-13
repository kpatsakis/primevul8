static void io_buffer_add_list(struct io_ring_ctx *ctx,
			       struct io_buffer_list *bl, unsigned int bgid)
{
	struct list_head *list;

	list = &ctx->io_buffers[hash_32(bgid, IO_BUFFERS_HASH_BITS)];
	INIT_LIST_HEAD(&bl->buf_list);
	bl->bgid = bgid;
	list_add(&bl->list, list);
}