static int handle_nop(struct kvm_vcpu *vcpu)
{
	return kvm_skip_emulated_instruction(vcpu);
}