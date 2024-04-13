static inline void kvm_apic_set_xapic_id(struct kvm_lapic *apic, u8 id)
{
	kvm_lapic_set_reg(apic, APIC_ID, id << 24);
	atomic_set_release(&apic->vcpu->kvm->arch.apic_map_dirty, DIRTY);
}