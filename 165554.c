static void start_apic_timer(struct kvm_lapic *apic)
{
	__start_apic_timer(apic, APIC_TMICT);
}