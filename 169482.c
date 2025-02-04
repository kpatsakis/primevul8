static int perf_event_set_clock(struct perf_event *event, clockid_t clk_id)
{
	bool nmi_safe = false;

	switch (clk_id) {
	case CLOCK_MONOTONIC:
		event->clock = &ktime_get_mono_fast_ns;
		nmi_safe = true;
		break;

	case CLOCK_MONOTONIC_RAW:
		event->clock = &ktime_get_raw_fast_ns;
		nmi_safe = true;
		break;

	case CLOCK_REALTIME:
		event->clock = &ktime_get_real_ns;
		break;

	case CLOCK_BOOTTIME:
		event->clock = &ktime_get_boottime_ns;
		break;

	case CLOCK_TAI:
		event->clock = &ktime_get_clocktai_ns;
		break;

	default:
		return -EINVAL;
	}

	if (!nmi_safe && !(event->pmu->capabilities & PERF_PMU_CAP_NO_NMI))
		return -EINVAL;

	return 0;
}