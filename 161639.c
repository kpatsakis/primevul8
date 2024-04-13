static void copy_workqueue_attrs(struct workqueue_attrs *to,
				 const struct workqueue_attrs *from)
{
	to->nice = from->nice;
	cpumask_copy(to->cpumask, from->cpumask);
	/*
	 * Unlike hash and equality test, this function doesn't ignore
	 * ->no_numa as it is used for both pool and wq attrs.  Instead,
	 * get_unbound_pool() explicitly clears ->no_numa after copying.
	 */
	to->no_numa = from->no_numa;
}