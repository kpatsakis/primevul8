static inline void kvm_apic_set_ldr(struct kvm_lapic *apic, u32 id)
{
	kvm_lapic_set_reg(apic, APIC_LDR, id);
	atomic_set_release(&apic->vcpu->kvm->arch.apic_map_dirty, DIRTY);
}