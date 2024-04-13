static struct workqueue_attrs *wq_sysfs_prep_attrs(struct workqueue_struct *wq)
{
	struct workqueue_attrs *attrs;

	lockdep_assert_held(&wq_pool_mutex);

	attrs = alloc_workqueue_attrs(GFP_KERNEL);
	if (!attrs)
		return NULL;

	copy_workqueue_attrs(attrs, wq->unbound_attrs);
	return attrs;
}