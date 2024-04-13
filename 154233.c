__checkparam_dl(const struct sched_attr *attr)
{
	/* deadline != 0 */
	if (attr->sched_deadline == 0)
		return false;

	/*
	 * Since we truncate DL_SCALE bits, make sure we're at least
	 * that big.
	 */
	if (attr->sched_runtime < (1ULL << DL_SCALE))
		return false;

	/*
	 * Since we use the MSB for wrap-around and sign issues, make
	 * sure it's not set (mind that period can be equal to zero).
	 */
	if (attr->sched_deadline & (1ULL << 63) ||
	    attr->sched_period & (1ULL << 63))
		return false;

	/* runtime <= deadline <= period (if period != 0) */
	if ((attr->sched_period != 0 &&
	     attr->sched_period < attr->sched_deadline) ||
	    attr->sched_deadline < attr->sched_runtime)
		return false;

	return true;
}