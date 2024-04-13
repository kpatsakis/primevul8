struct workqueue_attrs *alloc_workqueue_attrs(gfp_t gfp_mask)
{
	struct workqueue_attrs *attrs;

	attrs = kzalloc(sizeof(*attrs), gfp_mask);
	if (!attrs)
		goto fail;
	if (!alloc_cpumask_var(&attrs->cpumask, gfp_mask))
		goto fail;

	cpumask_copy(attrs->cpumask, cpu_possible_mask);
	return attrs;
fail:
	free_workqueue_attrs(attrs);
	return NULL;
}