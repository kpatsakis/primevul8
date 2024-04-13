static struct pool_workqueue *numa_pwq_tbl_install(struct workqueue_struct *wq,
						   int node,
						   struct pool_workqueue *pwq)
{
	struct pool_workqueue *old_pwq;

	lockdep_assert_held(&wq_pool_mutex);
	lockdep_assert_held(&wq->mutex);

	/* link_pwq() can handle duplicate calls */
	link_pwq(pwq);

	old_pwq = rcu_access_pointer(wq->numa_pwq_tbl[node]);
	rcu_assign_pointer(wq->numa_pwq_tbl[node], pwq);
	return old_pwq;
}