static int unit_get(struct idr *p, void *ptr)
{
	int unit, err;

again:
	if (!idr_pre_get(p, GFP_KERNEL)) {
		printk(KERN_ERR "PPP: No free memory for idr\n");
		return -ENOMEM;
	}

	err = idr_get_new_above(p, ptr, 0, &unit);
	if (err == -EAGAIN)
		goto again;

	return unit;
}