void kvm_inject_apic_timer_irqs(struct kvm_vcpu *vcpu)
{
	struct kvm_lapic *apic = vcpu->arch.apic;

	if (atomic_read(&apic->lapic_timer.pending) > 0) {
		kvm_apic_inject_pending_timer_irqs(apic);
		atomic_set(&apic->lapic_timer.pending, 0);
	}
}