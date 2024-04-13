bool kvm_can_use_hv_timer(struct kvm_vcpu *vcpu)
{
	return kvm_x86_ops.set_hv_timer
	       && !(kvm_mwait_in_guest(vcpu->kvm) ||
		    kvm_can_post_timer_interrupt(vcpu));
}