static inline void kvm_apic_set_dfr(struct kvm_lapic *apic, u32 val)
{
	kvm_lapic_set_reg(apic, APIC_DFR, val);
	atomic_set_release(&apic->vcpu->kvm->arch.apic_map_dirty, DIRTY);
}