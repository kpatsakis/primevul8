static struct worker *alloc_worker(int node)
{
	struct worker *worker;

	worker = kzalloc_node(sizeof(*worker), GFP_KERNEL, node);
	if (worker) {
		INIT_LIST_HEAD(&worker->entry);
		INIT_LIST_HEAD(&worker->scheduled);
		INIT_LIST_HEAD(&worker->node);
		/* on creation a worker is in !idle && prep state */
		worker->flags = WORKER_PREP;
	}
	return worker;
}