static void restart_apic_timer(struct kvm_lapic *apic)
{
	preempt_disable();

	if (!apic_lvtt_period(apic) && atomic_read(&apic->lapic_timer.pending))
		goto out;

	if (!start_hv_timer(apic))
		start_sw_timer(apic);
out:
	preempt_enable();
}