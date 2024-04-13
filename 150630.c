struct kvm_vcpu *preempt_notifier_to_vcpu(struct preempt_notifier *pn)
{
	return container_of(pn, struct kvm_vcpu, preempt_notifier);
}