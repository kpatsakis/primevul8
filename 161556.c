static ssize_t wq_numa_show(struct device *dev, struct device_attribute *attr,
			    char *buf)
{
	struct workqueue_struct *wq = dev_to_wq(dev);
	int written;

	mutex_lock(&wq->mutex);
	written = scnprintf(buf, PAGE_SIZE, "%d\n",
			    !wq->unbound_attrs->no_numa);
	mutex_unlock(&wq->mutex);

	return written;
}