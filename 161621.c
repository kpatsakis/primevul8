static void rcu_free_wq(struct rcu_head *rcu)
{
	struct workqueue_struct *wq =
		container_of(rcu, struct workqueue_struct, rcu);

	if (!(wq->flags & WQ_UNBOUND))
		free_percpu(wq->cpu_pwqs);
	else
		free_workqueue_attrs(wq->unbound_attrs);

	kfree(wq->rescuer);
	kfree(wq);
}