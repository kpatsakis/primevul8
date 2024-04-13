int schedule_on_each_cpu(work_func_t func)
{
	int cpu;
	struct work_struct __percpu *works;

	works = alloc_percpu(struct work_struct);
	if (!works)
		return -ENOMEM;

	get_online_cpus();

	for_each_online_cpu(cpu) {
		struct work_struct *work = per_cpu_ptr(works, cpu);

		INIT_WORK(work, func);
		schedule_work_on(cpu, work);
	}

	for_each_online_cpu(cpu)
		flush_work(per_cpu_ptr(works, cpu));

	put_online_cpus();
	free_percpu(works);
	return 0;
}