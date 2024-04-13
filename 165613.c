static void apic_timer_expired(struct kvm_lapic *apic, bool from_timer_fn)
{
	struct kvm_vcpu *vcpu = apic->vcpu;
	struct kvm_timer *ktimer = &apic->lapic_timer;

	if (atomic_read(&apic->lapic_timer.pending))
		return;

	if (apic_lvtt_tscdeadline(apic) || ktimer->hv_timer_in_use)
		ktimer->expired_tscdeadline = ktimer->tscdeadline;

	if (!from_timer_fn && vcpu->arch.apicv_active) {
		WARN_ON(kvm_get_running_vcpu() != vcpu);
		kvm_apic_inject_pending_timer_irqs(apic);
		return;
	}

	if (kvm_use_posted_timer_interrupt(apic->vcpu)) {
		/*
		 * Ensure the guest's timer has truly expired before posting an
		 * interrupt.  Open code the relevant checks to avoid querying
		 * lapic_timer_int_injected(), which will be false since the
		 * interrupt isn't yet injected.  Waiting until after injecting
		 * is not an option since that won't help a posted interrupt.
		 */
		if (vcpu->arch.apic->lapic_timer.expired_tscdeadline &&
		    vcpu->arch.apic->lapic_timer.timer_advance_ns)
			__kvm_wait_lapic_expire(vcpu);
		kvm_apic_inject_pending_timer_irqs(apic);
		return;
	}

	atomic_inc(&apic->lapic_timer.pending);
	kvm_make_request(KVM_REQ_UNBLOCK, vcpu);
	if (from_timer_fn)
		kvm_vcpu_kick(vcpu);
}