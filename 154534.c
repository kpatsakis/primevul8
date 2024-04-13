static int handle_vmcall(struct kvm_vcpu *vcpu)
{
	return kvm_emulate_hypercall(vcpu);
}