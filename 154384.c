static int handle_rdpmc(struct kvm_vcpu *vcpu)
{
	int err;

	err = kvm_rdpmc(vcpu);
	return kvm_complete_insn_gp(vcpu, err);
}