int perf_pmu_register(struct pmu *pmu, const char *name, int type)
{
	int cpu, ret, max = PERF_TYPE_MAX;

	mutex_lock(&pmus_lock);
	ret = -ENOMEM;
	pmu->pmu_disable_count = alloc_percpu(int);
	if (!pmu->pmu_disable_count)
		goto unlock;

	pmu->type = -1;
	if (!name)
		goto skip_type;
	pmu->name = name;

	if (type != PERF_TYPE_SOFTWARE) {
		if (type >= 0)
			max = type;

		ret = idr_alloc(&pmu_idr, pmu, max, 0, GFP_KERNEL);
		if (ret < 0)
			goto free_pdc;

		WARN_ON(type >= 0 && ret != type);

		type = ret;
	}
	pmu->type = type;

	if (pmu_bus_running) {
		ret = pmu_dev_alloc(pmu);
		if (ret)
			goto free_idr;
	}

skip_type:
	if (pmu->task_ctx_nr == perf_hw_context) {
		static int hw_context_taken = 0;

		/*
		 * Other than systems with heterogeneous CPUs, it never makes
		 * sense for two PMUs to share perf_hw_context. PMUs which are
		 * uncore must use perf_invalid_context.
		 */
		if (WARN_ON_ONCE(hw_context_taken &&
		    !(pmu->capabilities & PERF_PMU_CAP_HETEROGENEOUS_CPUS)))
			pmu->task_ctx_nr = perf_invalid_context;

		hw_context_taken = 1;
	}

	pmu->pmu_cpu_context = find_pmu_context(pmu->task_ctx_nr);
	if (pmu->pmu_cpu_context)
		goto got_cpu_context;

	ret = -ENOMEM;
	pmu->pmu_cpu_context = alloc_percpu(struct perf_cpu_context);
	if (!pmu->pmu_cpu_context)
		goto free_dev;

	for_each_possible_cpu(cpu) {
		struct perf_cpu_context *cpuctx;

		cpuctx = per_cpu_ptr(pmu->pmu_cpu_context, cpu);
		__perf_event_init_context(&cpuctx->ctx);
		lockdep_set_class(&cpuctx->ctx.mutex, &cpuctx_mutex);
		lockdep_set_class(&cpuctx->ctx.lock, &cpuctx_lock);
		cpuctx->ctx.pmu = pmu;
		cpuctx->online = cpumask_test_cpu(cpu, perf_online_mask);

		__perf_mux_hrtimer_init(cpuctx, cpu);

		cpuctx->heap_size = ARRAY_SIZE(cpuctx->heap_default);
		cpuctx->heap = cpuctx->heap_default;
	}

got_cpu_context:
	if (!pmu->start_txn) {
		if (pmu->pmu_enable) {
			/*
			 * If we have pmu_enable/pmu_disable calls, install
			 * transaction stubs that use that to try and batch
			 * hardware accesses.
			 */
			pmu->start_txn  = perf_pmu_start_txn;
			pmu->commit_txn = perf_pmu_commit_txn;
			pmu->cancel_txn = perf_pmu_cancel_txn;
		} else {
			pmu->start_txn  = perf_pmu_nop_txn;
			pmu->commit_txn = perf_pmu_nop_int;
			pmu->cancel_txn = perf_pmu_nop_void;
		}
	}

	if (!pmu->pmu_enable) {
		pmu->pmu_enable  = perf_pmu_nop_void;
		pmu->pmu_disable = perf_pmu_nop_void;
	}

	if (!pmu->check_period)
		pmu->check_period = perf_event_nop_int;

	if (!pmu->event_idx)
		pmu->event_idx = perf_event_idx_default;

	/*
	 * Ensure the TYPE_SOFTWARE PMUs are at the head of the list,
	 * since these cannot be in the IDR. This way the linear search
	 * is fast, provided a valid software event is provided.
	 */
	if (type == PERF_TYPE_SOFTWARE || !name)
		list_add_rcu(&pmu->entry, &pmus);
	else
		list_add_tail_rcu(&pmu->entry, &pmus);

	atomic_set(&pmu->exclusive_cnt, 0);
	ret = 0;
unlock:
	mutex_unlock(&pmus_lock);

	return ret;

free_dev:
	device_del(pmu->dev);
	put_device(pmu->dev);

free_idr:
	if (pmu->type != PERF_TYPE_SOFTWARE)
		idr_remove(&pmu_idr, pmu->type);

free_pdc:
	free_percpu(pmu->pmu_disable_count);
	goto unlock;
}