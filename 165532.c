int kvm_apic_get_state(struct kvm_vcpu *vcpu, struct kvm_lapic_state *s)
{
	memcpy(s->regs, vcpu->arch.apic->regs, sizeof(*s));

	/*
	 * Get calculated timer current count for remaining timer period (if
	 * any) and store it in the returned register set.
	 */
	__kvm_lapic_set_reg(s->regs, APIC_TMCCT,
			    __apic_read(vcpu->arch.apic, APIC_TMCCT));

	return kvm_apic_state_fixup(vcpu, s, false);
}