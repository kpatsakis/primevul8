static int cmm_timed_pages_handler(struct ctl_table *ctl, int write,
				   void __user *buffer, size_t *lenp,
				   loff_t *ppos)
{
	long nr = cmm_get_timed_pages();
	struct ctl_table ctl_entry = {
		.procname	= ctl->procname,
		.data		= &nr,
		.maxlen		= sizeof(long),
	};
	int rc;

	rc = proc_doulongvec_minmax(&ctl_entry, write, buffer, lenp, ppos);
	if (rc < 0 || !write)
		return rc;

	cmm_add_timed_pages(nr);
	return 0;
}