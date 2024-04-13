static void kvm_apic_inject_pending_timer_irqs(struct kvm_lapic *apic)
{
	struct kvm_timer *ktimer = &apic->lapic_timer;

	kvm_apic_local_deliver(apic, APIC_LVTT);
	if (apic_lvtt_tscdeadline(apic)) {
		ktimer->tscdeadline = 0;
	} else if (apic_lvtt_oneshot(apic)) {
		ktimer->tscdeadline = 0;
		ktimer->target_expiration = 0;
	}
}