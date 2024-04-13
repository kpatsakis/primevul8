static ssize_t wq_numa_store(struct device *dev, struct device_attribute *attr,
			     const char *buf, size_t count)
{
	struct workqueue_struct *wq = dev_to_wq(dev);
	struct workqueue_attrs *attrs;
	int v, ret = -ENOMEM;

	apply_wqattrs_lock();

	attrs = wq_sysfs_prep_attrs(wq);
	if (!attrs)
		goto out_unlock;

	ret = -EINVAL;
	if (sscanf(buf, "%d", &v) == 1) {
		attrs->no_numa = !v;
		ret = apply_workqueue_attrs_locked(wq, attrs);
	}

out_unlock:
	apply_wqattrs_unlock();
	free_workqueue_attrs(attrs);
	return ret ?: count;
}