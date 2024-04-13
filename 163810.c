static int unit_set(struct idr *p, void *ptr, int n)
{
	int unit, err;

again:
	if (!idr_pre_get(p, GFP_KERNEL)) {
		printk(KERN_ERR "PPP: No free memory for idr\n");
		return -ENOMEM;
	}

	err = idr_get_new_above(p, ptr, n, &unit);
	if (err == -EAGAIN)
		goto again;

	if (unit != n) {
		idr_remove(p, unit);
		return -EINVAL;
	}

	return unit;
}