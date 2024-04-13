 */
static __cold bool io_poll_remove_all(struct io_ring_ctx *ctx,
				      struct task_struct *tsk, bool cancel_all)
{
	struct hlist_node *tmp;
	struct io_kiocb *req;
	bool found = false;
	int i;

	spin_lock(&ctx->completion_lock);
	for (i = 0; i < (1U << ctx->cancel_hash_bits); i++) {
		struct hlist_head *list;

		list = &ctx->cancel_hash[i];
		hlist_for_each_entry_safe(req, tmp, list, hash_node) {
			if (io_match_task_safe(req, tsk, cancel_all)) {
				hlist_del_init(&req->hash_node);
				io_poll_cancel_req(req);
				found = true;
			}
		}
	}
	spin_unlock(&ctx->completion_lock);
	return found;