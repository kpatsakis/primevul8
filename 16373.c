static struct io_buffer_list *io_buffer_get_list(struct io_ring_ctx *ctx,
						 unsigned int bgid)
{
	struct list_head *hash_list;
	struct io_buffer_list *bl;

	hash_list = &ctx->io_buffers[hash_32(bgid, IO_BUFFERS_HASH_BITS)];
	list_for_each_entry(bl, hash_list, list)
		if (bl->bgid == bgid || bgid == -1U)
			return bl;

	return NULL;
}