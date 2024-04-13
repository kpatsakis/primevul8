static void vmx_clear_hlt(struct kvm_vcpu *vcpu)
{
	/*
	 * Ensure that we clear the HLT state in the VMCS.  We don't need to
	 * explicitly skip the instruction because if the HLT state is set,
	 * then the instruction is already executing and RIP has already been
	 * advanced.
	 */
	if (kvm_hlt_in_guest(vcpu->kvm) &&
			vmcs_read32(GUEST_ACTIVITY_STATE) == GUEST_ACTIVITY_HLT)
		vmcs_write32(GUEST_ACTIVITY_STATE, GUEST_ACTIVITY_ACTIVE);
}