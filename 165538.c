void __kvm_migrate_apic_timer(struct kvm_vcpu *vcpu)
{
	struct hrtimer *timer;

	if (!lapic_in_kernel(vcpu) ||
		kvm_can_post_timer_interrupt(vcpu))
		return;

	timer = &vcpu->arch.apic->lapic_timer.timer;
	if (hrtimer_cancel(timer))
		hrtimer_start_expires(timer, HRTIMER_MODE_ABS_HARD);
}