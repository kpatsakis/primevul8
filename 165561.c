static void __kvm_wait_lapic_expire(struct kvm_vcpu *vcpu)
{
	struct kvm_lapic *apic = vcpu->arch.apic;
	u64 guest_tsc, tsc_deadline;

	tsc_deadline = apic->lapic_timer.expired_tscdeadline;
	apic->lapic_timer.expired_tscdeadline = 0;
	guest_tsc = kvm_read_l1_tsc(vcpu, rdtsc());
	apic->lapic_timer.advance_expire_delta = guest_tsc - tsc_deadline;

	if (lapic_timer_advance_dynamic) {
		adjust_lapic_timer_advance(vcpu, apic->lapic_timer.advance_expire_delta);
		/*
		 * If the timer fired early, reread the TSC to account for the
		 * overhead of the above adjustment to avoid waiting longer
		 * than is necessary.
		 */
		if (guest_tsc < tsc_deadline)
			guest_tsc = kvm_read_l1_tsc(vcpu, rdtsc());
	}

	if (guest_tsc < tsc_deadline)
		__wait_lapic_expire(vcpu, tsc_deadline - guest_tsc);
}