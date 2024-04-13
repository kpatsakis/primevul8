static int wq_clamp_max_active(int max_active, unsigned int flags,
			       const char *name)
{
	int lim = flags & WQ_UNBOUND ? WQ_UNBOUND_MAX_ACTIVE : WQ_MAX_ACTIVE;

	if (max_active < 1 || max_active > lim)
		pr_warn("workqueue: max_active %d requested for %s is out of range, clamping between %d and %d\n",
			max_active, name, 1, lim);

	return clamp_val(max_active, 1, lim);
}