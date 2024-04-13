static struct worker *first_idle_worker(struct worker_pool *pool)
{
	if (unlikely(list_empty(&pool->idle_list)))
		return NULL;

	return list_first_entry(&pool->idle_list, struct worker, entry);
}