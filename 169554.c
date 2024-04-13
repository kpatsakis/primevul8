__perf_event_account_interrupt(struct perf_event *event, int throttle)
{
	struct hw_perf_event *hwc = &event->hw;
	int ret = 0;
	u64 seq;

	seq = __this_cpu_read(perf_throttled_seq);
	if (seq != hwc->interrupts_seq) {
		hwc->interrupts_seq = seq;
		hwc->interrupts = 1;
	} else {
		hwc->interrupts++;
		if (unlikely(throttle
			     && hwc->interrupts >= max_samples_per_tick)) {
			__this_cpu_inc(perf_throttled_count);
			tick_dep_set_cpu(smp_processor_id(), TICK_DEP_BIT_PERF_EVENTS);
			hwc->interrupts = MAX_INTERRUPTS;
			perf_log_throttle(event, 0);
			ret = 1;
		}
	}

	if (event->attr.freq) {
		u64 now = perf_clock();
		s64 delta = now - hwc->freq_time_stamp;

		hwc->freq_time_stamp = now;

		if (delta > 0 && delta < 2*TICK_NSEC)
			perf_adjust_period(event, delta, hwc->last_period, true);
	}

	return ret;
}