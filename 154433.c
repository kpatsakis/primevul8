static int handle_invalid_op(struct kvm_vcpu *vcpu)
{
	kvm_queue_exception(vcpu, UD_VECTOR);
	return 1;
}