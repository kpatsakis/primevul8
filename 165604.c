int kvm_apic_has_interrupt(struct kvm_vcpu *vcpu)
{
	struct kvm_lapic *apic = vcpu->arch.apic;
	u32 ppr;

	if (!kvm_apic_present(vcpu))
		return -1;

	__apic_update_ppr(apic, &ppr);
	return apic_has_interrupt_for_ppr(apic, ppr);
}