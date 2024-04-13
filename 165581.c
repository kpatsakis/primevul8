static inline u64 tmict_to_ns(struct kvm_lapic *apic, u32 tmict)
{
	return (u64)tmict * APIC_BUS_CYCLE_NS * (u64)apic->divide_count;
}