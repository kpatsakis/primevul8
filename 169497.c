void __init perf_event_init(void)
{
	int ret;

	idr_init(&pmu_idr);

	perf_event_init_all_cpus();
	init_srcu_struct(&pmus_srcu);
	perf_pmu_register(&perf_swevent, "software", PERF_TYPE_SOFTWARE);
	perf_pmu_register(&perf_cpu_clock, NULL, -1);
	perf_pmu_register(&perf_task_clock, NULL, -1);
	perf_tp_register();
	perf_event_init_cpu(smp_processor_id());
	register_reboot_notifier(&perf_reboot_notifier);

	ret = init_hw_breakpoint();
	WARN(ret, "hw_breakpoint initialization failed with: %d", ret);

	/*
	 * Build time assertion that we keep the data_head at the intended
	 * location.  IOW, validation we got the __reserved[] size right.
	 */
	BUILD_BUG_ON((offsetof(struct perf_event_mmap_page, data_head))
		     != 1024);
}