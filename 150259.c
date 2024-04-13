static int ucma_close(struct inode *inode, struct file *filp)
{
	struct ucma_file *file = filp->private_data;

	/*
	 * All paths that touch ctx_list or ctx_list starting from write() are
	 * prevented by this being a FD release function. The list_add_tail() in
	 * ucma_connect_event_handler() can run concurrently, however it only
	 * adds to the list *after* a listening ID. By only reading the first of
	 * the list, and relying on __destroy_id() to block
	 * ucma_connect_event_handler(), no additional locking is needed.
	 */
	while (!list_empty(&file->ctx_list)) {
		struct ucma_context *ctx = list_first_entry(
			&file->ctx_list, struct ucma_context, list);

		xa_erase(&ctx_table, ctx->id);
		__destroy_id(ctx);
	}
	kfree(file);
	return 0;
}