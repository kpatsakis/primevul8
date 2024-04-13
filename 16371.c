
static void io_destroy_buffers(struct io_ring_ctx *ctx)
{
	int i;

	for (i = 0; i < (1U << IO_BUFFERS_HASH_BITS); i++) {
		struct list_head *list = &ctx->io_buffers[i];

		while (!list_empty(list)) {
			struct io_buffer_list *bl;

			bl = list_first_entry(list, struct io_buffer_list, list);
			__io_remove_buffers(ctx, bl, -1U);
			list_del(&bl->list);
			kfree(bl);
		}
	}

	while (!list_empty(&ctx->io_buffers_pages)) {
		struct page *page;

		page = list_first_entry(&ctx->io_buffers_pages, struct page, lru);
		list_del_init(&page->lru);
		__free_page(page);
	}