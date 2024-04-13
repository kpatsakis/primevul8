static void ucma_cleanup_multicast(struct ucma_context *ctx)
{
	struct ucma_multicast *mc;
	unsigned long index;

	xa_for_each(&multicast_table, index, mc) {
		if (mc->ctx != ctx)
			continue;
		/*
		 * At this point mc->ctx->ref is 0 so the mc cannot leave the
		 * lock on the reader and this is enough serialization
		 */
		xa_erase(&multicast_table, index);
		kfree(mc);
	}
}