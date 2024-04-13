static void cancel_apic_timer(struct kvm_lapic *apic)
{
	hrtimer_cancel(&apic->lapic_timer.timer);
	preempt_disable();
	if (apic->lapic_timer.hv_timer_in_use)
		cancel_hv_timer(apic);
	preempt_enable();
}