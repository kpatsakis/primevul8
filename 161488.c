int hrtimers_prepare_cpu(unsigned int cpu)
{
	struct hrtimer_cpu_base *cpu_base = &per_cpu(hrtimer_bases, cpu);
	int i;

	for (i = 0; i < HRTIMER_MAX_CLOCK_BASES; i++) {
		cpu_base->clock_base[i].cpu_base = cpu_base;
		timerqueue_init_head(&cpu_base->clock_base[i].active);
	}

	cpu_base->cpu = cpu;
	hrtimer_init_hres(cpu_base);
	return 0;
}