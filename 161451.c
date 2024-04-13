static struct pool_workqueue *unbound_pwq_by_node(struct workqueue_struct *wq,
						  int node)
{
	assert_rcu_or_wq_mutex_or_pool_mutex(wq);

	/*
	 * XXX: @node can be NUMA_NO_NODE if CPU goes offline while a
	 * delayed item is pending.  The plan is to keep CPU -> NODE
	 * mapping valid and stable across CPU on/offlines.  Once that
	 * happens, this workaround can be removed.
	 */
	if (unlikely(node == NUMA_NO_NODE))
		return wq->dfl_pwq;

	return rcu_dereference_raw(wq->numa_pwq_tbl[node]);
}