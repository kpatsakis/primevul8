static void __init init_timer_cpus(void)
{
	int cpu;

	for_each_possible_cpu(cpu)
		init_timer_cpu(cpu);
}