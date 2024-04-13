static void vmx_hwapic_isr_update(struct kvm_vcpu *vcpu, int max_isr)
{
	u16 status;
	u8 old;

	if (max_isr == -1)
		max_isr = 0;

	status = vmcs_read16(GUEST_INTR_STATUS);
	old = status >> 8;
	if (max_isr != old) {
		status &= 0xff;
		status |= max_isr << 8;
		vmcs_write16(GUEST_INTR_STATUS, status);
	}
}