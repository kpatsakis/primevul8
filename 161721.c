static ssize_t per_cpu_show(struct device *dev, struct device_attribute *attr,
			    char *buf)
{
	struct workqueue_struct *wq = dev_to_wq(dev);

	return scnprintf(buf, PAGE_SIZE, "%d\n", (bool)!(wq->flags & WQ_UNBOUND));
}