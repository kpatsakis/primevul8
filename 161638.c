void free_workqueue_attrs(struct workqueue_attrs *attrs)
{
	if (attrs) {
		free_cpumask_var(attrs->cpumask);
		kfree(attrs);
	}
}