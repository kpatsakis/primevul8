static ssize_t wq_pool_ids_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	struct workqueue_struct *wq = dev_to_wq(dev);
	const char *delim = "";
	int node, written = 0;

	rcu_read_lock_sched();
	for_each_node(node) {
		written += scnprintf(buf + written, PAGE_SIZE - written,
				     "%s%d:%d", delim, node,
				     unbound_pwq_by_node(wq, node)->pool->id);
		delim = " ";
	}
	written += scnprintf(buf + written, PAGE_SIZE - written, "\n");
	rcu_read_unlock_sched();

	return written;
}