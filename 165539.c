static void start_sw_period(struct kvm_lapic *apic)
{
	if (!apic->lapic_timer.period)
		return;

	if (ktime_after(ktime_get(),
			apic->lapic_timer.target_expiration)) {
		apic_timer_expired(apic, false);

		if (apic_lvtt_oneshot(apic))
			return;

		advance_periodic_target_expiration(apic);
	}

	hrtimer_start(&apic->lapic_timer.timer,
		apic->lapic_timer.target_expiration,
		HRTIMER_MODE_ABS_HARD);
}