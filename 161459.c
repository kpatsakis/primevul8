static void __init init_timer_cpu(int cpu)
{
	struct timer_base *base;
	int i;

	for (i = 0; i < NR_BASES; i++) {
		base = per_cpu_ptr(&timer_bases[i], cpu);
		base->cpu = cpu;
		spin_lock_init(&base->lock);
		base->clk = jiffies;
	}
}