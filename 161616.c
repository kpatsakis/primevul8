static inline void timer_list_header(struct seq_file *m, u64 now)
{
	SEQ_printf(m, "Timer List Version: v0.8\n");
	SEQ_printf(m, "HRTIMER_MAX_CLOCK_BASES: %d\n", HRTIMER_MAX_CLOCK_BASES);
	SEQ_printf(m, "now at %Ld nsecs\n", (unsigned long long)now);
	SEQ_printf(m, "\n");
}