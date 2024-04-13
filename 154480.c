static int handle_wbinvd(struct kvm_vcpu *vcpu)
{
	return kvm_emulate_wbinvd(vcpu);
}