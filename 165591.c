static void apic_update_lvtt(struct kvm_lapic *apic)
{
	u32 timer_mode = kvm_lapic_get_reg(apic, APIC_LVTT) &
			apic->lapic_timer.timer_mode_mask;

	if (apic->lapic_timer.timer_mode != timer_mode) {
		if (apic_lvtt_tscdeadline(apic) != (timer_mode ==
				APIC_LVT_TIMER_TSCDEADLINE)) {
			cancel_apic_timer(apic);
			kvm_lapic_set_reg(apic, APIC_TMICT, 0);
			apic->lapic_timer.period = 0;
			apic->lapic_timer.tscdeadline = 0;
		}
		apic->lapic_timer.timer_mode = timer_mode;
		limit_periodic_timer_frequency(apic);
	}
}