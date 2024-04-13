static inline void adjust_lapic_timer_advance(struct kvm_vcpu *vcpu,
					      s64 advance_expire_delta)
{
	struct kvm_lapic *apic = vcpu->arch.apic;
	u32 timer_advance_ns = apic->lapic_timer.timer_advance_ns;
	u64 ns;

	/* Do not adjust for tiny fluctuations or large random spikes. */
	if (abs(advance_expire_delta) > LAPIC_TIMER_ADVANCE_ADJUST_MAX ||
	    abs(advance_expire_delta) < LAPIC_TIMER_ADVANCE_ADJUST_MIN)
		return;

	/* too early */
	if (advance_expire_delta < 0) {
		ns = -advance_expire_delta * 1000000ULL;
		do_div(ns, vcpu->arch.virtual_tsc_khz);
		timer_advance_ns -= ns/LAPIC_TIMER_ADVANCE_ADJUST_STEP;
	} else {
	/* too late */
		ns = advance_expire_delta * 1000000ULL;
		do_div(ns, vcpu->arch.virtual_tsc_khz);
		timer_advance_ns += ns/LAPIC_TIMER_ADVANCE_ADJUST_STEP;
	}

	if (unlikely(timer_advance_ns > LAPIC_TIMER_ADVANCE_NS_MAX))
		timer_advance_ns = LAPIC_TIMER_ADVANCE_NS_INIT;
	apic->lapic_timer.timer_advance_ns = timer_advance_ns;
}