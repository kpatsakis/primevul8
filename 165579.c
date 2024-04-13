void kvm_apic_set_version(struct kvm_vcpu *vcpu)
{
	struct kvm_lapic *apic = vcpu->arch.apic;
	u32 v = APIC_VERSION;

	if (!lapic_in_kernel(vcpu))
		return;

	/*
	 * KVM emulates 82093AA datasheet (with in-kernel IOAPIC implementation)
	 * which doesn't have EOI register; Some buggy OSes (e.g. Windows with
	 * Hyper-V role) disable EOI broadcast in lapic not checking for IOAPIC
	 * version first and level-triggered interrupts never get EOIed in
	 * IOAPIC.
	 */
	if (guest_cpuid_has(vcpu, X86_FEATURE_X2APIC) &&
	    !ioapic_in_kernel(vcpu->kvm))
		v |= APIC_LVR_DIRECTED_EOI;
	kvm_lapic_set_reg(apic, APIC_LVR, v);
}