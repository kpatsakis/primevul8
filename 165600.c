int kvm_get_apic_interrupt(struct kvm_vcpu *vcpu)
{
	int vector = kvm_apic_has_interrupt(vcpu);
	struct kvm_lapic *apic = vcpu->arch.apic;
	u32 ppr;

	if (vector == -1)
		return -1;

	/*
	 * We get here even with APIC virtualization enabled, if doing
	 * nested virtualization and L1 runs with the "acknowledge interrupt
	 * on exit" mode.  Then we cannot inject the interrupt via RVI,
	 * because the process would deliver it through the IDT.
	 */

	apic_clear_irr(vector, apic);
	if (to_hv_vcpu(vcpu) && test_bit(vector, to_hv_synic(vcpu)->auto_eoi_bitmap)) {
		/*
		 * For auto-EOI interrupts, there might be another pending
		 * interrupt above PPR, so check whether to raise another
		 * KVM_REQ_EVENT.
		 */
		apic_update_ppr(apic);
	} else {
		/*
		 * For normal interrupts, PPR has been raised and there cannot
		 * be a higher-priority pending interrupt---except if there was
		 * a concurrent interrupt injection, but that would have
		 * triggered KVM_REQ_EVENT already.
		 */
		apic_set_isr(vector, apic);
		__apic_update_ppr(apic, &ppr);
	}

	return vector;
}