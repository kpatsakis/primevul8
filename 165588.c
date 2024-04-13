int kvm_apic_accept_pic_intr(struct kvm_vcpu *vcpu)
{
	u32 lvt0 = kvm_lapic_get_reg(vcpu->arch.apic, APIC_LVT0);

	if (!kvm_apic_hw_enabled(vcpu->arch.apic))
		return 1;
	if ((lvt0 & APIC_LVT_MASKED) == 0 &&
	    GET_APIC_DELIVERY_MODE(lvt0) == APIC_MODE_EXTINT)
		return 1;
	return 0;
}