static int wq_watchdog_param_set_thresh(const char *val,
					const struct kernel_param *kp)
{
	unsigned long thresh;
	int ret;

	ret = kstrtoul(val, 0, &thresh);
	if (ret)
		return ret;

	if (system_wq)
		wq_watchdog_set_thresh(thresh);
	else
		wq_watchdog_thresh = thresh;

	return 0;
}