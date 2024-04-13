long work_on_cpu(int cpu, long (*fn)(void *), void *arg)
{
	struct work_for_cpu wfc = { .fn = fn, .arg = arg };

	INIT_WORK_ONSTACK(&wfc.work, work_for_cpu_fn);
	schedule_work_on(cpu, &wfc.work);
	flush_work(&wfc.work);
	destroy_work_on_stack(&wfc.work);
	return wfc.ret;
}