static void timer_list_show_tickdevices_header(struct seq_file *m)
{
#ifdef CONFIG_GENERIC_CLOCKEVENTS_BROADCAST
	print_tickdevice(m, tick_get_broadcast_device(), -1);
	SEQ_printf(m, "tick_broadcast_mask: %*pb\n",
		   cpumask_pr_args(tick_get_broadcast_mask()));
#ifdef CONFIG_TICK_ONESHOT
	SEQ_printf(m, "tick_broadcast_oneshot_mask: %*pb\n",
		   cpumask_pr_args(tick_get_broadcast_oneshot_mask()));
#endif
	SEQ_printf(m, "\n");
#endif
}