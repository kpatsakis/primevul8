static inline void __wait_lapic_expire(struct kvm_vcpu *vcpu, u64 guest_cycles)
{
	u64 timer_advance_ns = vcpu->arch.apic->lapic_timer.timer_advance_ns;

	/*
	 * If the guest TSC is running at a different ratio than the host, then
	 * convert the delay to nanoseconds to achieve an accurate delay.  Note
	 * that __delay() uses delay_tsc whenever the hardware has TSC, thus
	 * always for VMX enabled hardware.
	 */
	if (vcpu->arch.tsc_scaling_ratio == kvm_default_tsc_scaling_ratio) {
		__delay(min(guest_cycles,
			nsec_to_cycles(vcpu, timer_advance_ns)));
	} else {
		u64 delay_ns = guest_cycles * 1000000ULL;
		do_div(delay_ns, vcpu->arch.virtual_tsc_khz);
		ndelay(min_t(u32, delay_ns, timer_advance_ns));
	}
}