int apply_workqueue_attrs(struct workqueue_struct *wq,
			  const struct workqueue_attrs *attrs)
{
	int ret;

	apply_wqattrs_lock();
	ret = apply_workqueue_attrs_locked(wq, attrs);
	apply_wqattrs_unlock();

	return ret;
}