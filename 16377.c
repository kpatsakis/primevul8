
static struct io_wq *io_init_wq_offload(struct io_ring_ctx *ctx,
					struct task_struct *task)
{
	struct io_wq_hash *hash;
	struct io_wq_data data;
	unsigned int concurrency;

	mutex_lock(&ctx->uring_lock);
	hash = ctx->hash_map;
	if (!hash) {
		hash = kzalloc(sizeof(*hash), GFP_KERNEL);
		if (!hash) {
			mutex_unlock(&ctx->uring_lock);
			return ERR_PTR(-ENOMEM);
		}
		refcount_set(&hash->refs, 1);
		init_waitqueue_head(&hash->wait);
		ctx->hash_map = hash;
	}
	mutex_unlock(&ctx->uring_lock);

	data.hash = hash;
	data.task = task;
	data.free_work = io_wq_free_work;
	data.do_work = io_wq_submit_work;

	/* Do QD, or 4 * CPUS, whatever is smallest */
	concurrency = min(ctx->sq_entries, 4 * num_online_cpus());

	return io_wq_create(concurrency, &data);