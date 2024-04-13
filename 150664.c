void kvm_resched(struct kvm_vcpu *vcpu)
{
	if (!need_resched())
		return;
	cond_resched();
}