void kvm_wait_lapic_expire(struct kvm_vcpu *vcpu)
{
	if (lapic_in_kernel(vcpu) &&
	    vcpu->arch.apic->lapic_timer.expired_tscdeadline &&
	    vcpu->arch.apic->lapic_timer.timer_advance_ns &&
	    lapic_timer_int_injected(vcpu))
		__kvm_wait_lapic_expire(vcpu);
}