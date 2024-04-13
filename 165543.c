static void __start_apic_timer(struct kvm_lapic *apic, u32 count_reg)
{
	atomic_set(&apic->lapic_timer.pending, 0);

	if ((apic_lvtt_period(apic) || apic_lvtt_oneshot(apic))
	    && !set_target_expiration(apic, count_reg))
		return;

	restart_apic_timer(apic);
}