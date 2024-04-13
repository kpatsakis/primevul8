static bool start_hv_timer(struct kvm_lapic *apic)
{
	struct kvm_timer *ktimer = &apic->lapic_timer;
	struct kvm_vcpu *vcpu = apic->vcpu;
	bool expired;

	WARN_ON(preemptible());
	if (!kvm_can_use_hv_timer(vcpu))
		return false;

	if (!ktimer->tscdeadline)
		return false;

	if (static_call(kvm_x86_set_hv_timer)(vcpu, ktimer->tscdeadline, &expired))
		return false;

	ktimer->hv_timer_in_use = true;
	hrtimer_cancel(&ktimer->timer);

	/*
	 * To simplify handling the periodic timer, leave the hv timer running
	 * even if the deadline timer has expired, i.e. rely on the resulting
	 * VM-Exit to recompute the periodic timer's target expiration.
	 */
	if (!apic_lvtt_period(apic)) {
		/*
		 * Cancel the hv timer if the sw timer fired while the hv timer
		 * was being programmed, or if the hv timer itself expired.
		 */
		if (atomic_read(&ktimer->pending)) {
			cancel_hv_timer(apic);
		} else if (expired) {
			apic_timer_expired(apic, false);
			cancel_hv_timer(apic);
		}
	}

	trace_kvm_hv_timer_state(vcpu->vcpu_id, ktimer->hv_timer_in_use);

	return true;
}