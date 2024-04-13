static int vmx_pre_block(struct kvm_vcpu *vcpu)
{
	if (pi_pre_block(vcpu))
		return 1;

	if (kvm_lapic_hv_timer_in_use(vcpu))
		kvm_lapic_switch_to_sw_timer(vcpu);

	return 0;
}