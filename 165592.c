static bool kvm_use_posted_timer_interrupt(struct kvm_vcpu *vcpu)
{
	return kvm_can_post_timer_interrupt(vcpu) && vcpu->mode == IN_GUEST_MODE;
}