static void wake_up_worker(struct worker_pool *pool)
{
	struct worker *worker = first_idle_worker(pool);

	if (likely(worker))
		wake_up_process(worker->task);
}