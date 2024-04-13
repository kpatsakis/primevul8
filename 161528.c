static int __init init_timer_list_procfs(void)
{
	struct proc_dir_entry *pe;

	pe = proc_create("timer_list", 0444, NULL, &timer_list_fops);
	if (!pe)
		return -ENOMEM;
	return 0;
}