void __kvm_migrate_pit_timer(struct kvm_vcpu *vcpu)
{
	struct kvm_pit *pit = vcpu->kvm->arch.vpit;
	struct hrtimer *timer;

	if (!kvm_vcpu_is_bsp(vcpu) || !pit)
		return;

	timer = &pit->pit_state.timer;
	mutex_lock(&pit->pit_state.lock);
	if (hrtimer_cancel(timer))
		hrtimer_start_expires(timer, HRTIMER_MODE_ABS);
	mutex_unlock(&pit->pit_state.lock);
}