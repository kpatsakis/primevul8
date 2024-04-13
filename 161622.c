static ssize_t wq_nice_store(struct device *dev, struct device_attribute *attr,
			     const char *buf, size_t count)
{
	struct workqueue_struct *wq = dev_to_wq(dev);
	struct workqueue_attrs *attrs;
	int ret = -ENOMEM;

	apply_wqattrs_lock();

	attrs = wq_sysfs_prep_attrs(wq);
	if (!attrs)
		goto out_unlock;

	if (sscanf(buf, "%d", &attrs->nice) == 1 &&
	    attrs->nice >= MIN_NICE && attrs->nice <= MAX_NICE)
		ret = apply_workqueue_attrs_locked(wq, attrs);
	else
		ret = -EINVAL;

out_unlock:
	apply_wqattrs_unlock();
	free_workqueue_attrs(attrs);
	return ret ?: count;
}