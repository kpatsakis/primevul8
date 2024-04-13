u64 kvm_get_lapic_tscdeadline_msr(struct kvm_vcpu *vcpu)
{
	struct kvm_lapic *apic = vcpu->arch.apic;

	if (!kvm_apic_present(vcpu) || !apic_lvtt_tscdeadline(apic))
		return 0;

	return apic->lapic_timer.tscdeadline;
}