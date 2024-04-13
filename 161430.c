print_base(struct seq_file *m, struct hrtimer_clock_base *base, u64 now)
{
	SEQ_printf(m, "  .base:       %pK\n", base);
	SEQ_printf(m, "  .index:      %d\n", base->index);

	SEQ_printf(m, "  .resolution: %u nsecs\n", (unsigned) hrtimer_resolution);

	SEQ_printf(m,   "  .get_time:   ");
	print_name_offset(m, base->get_time);
	SEQ_printf(m,   "\n");
#ifdef CONFIG_HIGH_RES_TIMERS
	SEQ_printf(m, "  .offset:     %Lu nsecs\n",
		   (unsigned long long) ktime_to_ns(base->offset));
#endif
	SEQ_printf(m,   "active timers:\n");
	print_active_timers(m, base, now + ktime_to_ns(base->offset));
}