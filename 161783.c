static void print_cpu(struct seq_file *m, int cpu, u64 now)
{
	struct hrtimer_cpu_base *cpu_base = &per_cpu(hrtimer_bases, cpu);
	int i;

	SEQ_printf(m, "cpu: %d\n", cpu);
	for (i = 0; i < HRTIMER_MAX_CLOCK_BASES; i++) {
		SEQ_printf(m, " clock %d:\n", i);
		print_base(m, cpu_base->clock_base + i, now);
	}
#define P(x) \
	SEQ_printf(m, "  .%-15s: %Lu\n", #x, \
		   (unsigned long long)(cpu_base->x))
#define P_ns(x) \
	SEQ_printf(m, "  .%-15s: %Lu nsecs\n", #x, \
		   (unsigned long long)(ktime_to_ns(cpu_base->x)))

#ifdef CONFIG_HIGH_RES_TIMERS
	P_ns(expires_next);
	P(hres_active);
	P(nr_events);
	P(nr_retries);
	P(nr_hangs);
	P(max_hang_time);
#endif
#undef P
#undef P_ns

#ifdef CONFIG_TICK_ONESHOT
# define P(x) \
	SEQ_printf(m, "  .%-15s: %Lu\n", #x, \
		   (unsigned long long)(ts->x))
# define P_ns(x) \
	SEQ_printf(m, "  .%-15s: %Lu nsecs\n", #x, \
		   (unsigned long long)(ktime_to_ns(ts->x)))
	{
		struct tick_sched *ts = tick_get_tick_sched(cpu);
		P(nohz_mode);
		P_ns(last_tick);
		P(tick_stopped);
		P(idle_jiffies);
		P(idle_calls);
		P(idle_sleeps);
		P_ns(idle_entrytime);
		P_ns(idle_waketime);
		P_ns(idle_exittime);
		P_ns(idle_sleeptime);
		P_ns(iowait_sleeptime);
		P(last_jiffies);
		P(next_timer);
		P_ns(idle_expires);
		SEQ_printf(m, "jiffies: %Lu\n",
			   (unsigned long long)jiffies);
	}
#endif

#undef P
#undef P_ns
	SEQ_printf(m, "\n");
}