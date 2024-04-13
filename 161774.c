static ssize_t wq_unbound_cpumask_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	cpumask_var_t cpumask;
	int ret;

	if (!zalloc_cpumask_var(&cpumask, GFP_KERNEL))
		return -ENOMEM;

	ret = cpumask_parse(buf, cpumask);
	if (!ret)
		ret = workqueue_set_unbound_cpumask(cpumask);

	free_cpumask_var(cpumask);
	return ret ? ret : count;
}