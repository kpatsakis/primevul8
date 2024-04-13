
static __cold bool io_cancel_defer_files(struct io_ring_ctx *ctx,
					 struct task_struct *task,
					 bool cancel_all)
{
	struct io_defer_entry *de;
	LIST_HEAD(list);

	spin_lock(&ctx->completion_lock);
	list_for_each_entry_reverse(de, &ctx->defer_list, list) {
		if (io_match_task_safe(de->req, task, cancel_all)) {
			list_cut_position(&list, &ctx->defer_list, &de->list);
			break;
		}
	}
	spin_unlock(&ctx->completion_lock);
	if (list_empty(&list))
		return false;

	while (!list_empty(&list)) {
		de = list_first_entry(&list, struct io_defer_entry, list);
		list_del_init(&de->list);
		io_req_complete_failed(de->req, -ECANCELED);
		kfree(de);
	}
	return true;