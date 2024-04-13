
static void io_flush_apoll_cache(struct io_ring_ctx *ctx)
{
	struct async_poll *apoll;

	while (!list_empty(&ctx->apoll_cache)) {
		apoll = list_first_entry(&ctx->apoll_cache, struct async_poll,
						poll.wait.entry);
		list_del(&apoll->poll.wait.entry);
		kfree(apoll);
	}