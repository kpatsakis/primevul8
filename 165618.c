static bool set_target_expiration(struct kvm_lapic *apic, u32 count_reg)
{
	ktime_t now;
	u64 tscl = rdtsc();
	s64 deadline;

	now = ktime_get();
	apic->lapic_timer.period =
			tmict_to_ns(apic, kvm_lapic_get_reg(apic, APIC_TMICT));

	if (!apic->lapic_timer.period) {
		apic->lapic_timer.tscdeadline = 0;
		return false;
	}

	limit_periodic_timer_frequency(apic);
	deadline = apic->lapic_timer.period;

	if (apic_lvtt_period(apic) || apic_lvtt_oneshot(apic)) {
		if (unlikely(count_reg != APIC_TMICT)) {
			deadline = tmict_to_ns(apic,
				     kvm_lapic_get_reg(apic, count_reg));
			if (unlikely(deadline <= 0))
				deadline = apic->lapic_timer.period;
			else if (unlikely(deadline > apic->lapic_timer.period)) {
				pr_info_ratelimited(
				    "kvm: vcpu %i: requested lapic timer restore with "
				    "starting count register %#x=%u (%lld ns) > initial count (%lld ns). "
				    "Using initial count to start timer.\n",
				    apic->vcpu->vcpu_id,
				    count_reg,
				    kvm_lapic_get_reg(apic, count_reg),
				    deadline, apic->lapic_timer.period);
				kvm_lapic_set_reg(apic, count_reg, 0);
				deadline = apic->lapic_timer.period;
			}
		}
	}

	apic->lapic_timer.tscdeadline = kvm_read_l1_tsc(apic->vcpu, tscl) +
		nsec_to_cycles(apic->vcpu, deadline);
	apic->lapic_timer.target_expiration = ktime_add_ns(now, deadline);

	return true;
}