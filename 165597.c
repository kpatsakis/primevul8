static void cancel_hv_timer(struct kvm_lapic *apic)
{
	WARN_ON(preemptible());
	WARN_ON(!apic->lapic_timer.hv_timer_in_use);
	static_call(kvm_x86_cancel_hv_timer)(apic->vcpu);
	apic->lapic_timer.hv_timer_in_use = false;
}