int kvm_pv_send_ipi(struct kvm *kvm, unsigned long ipi_bitmap_low,
		    unsigned long ipi_bitmap_high, u32 min,
		    unsigned long icr, int op_64_bit)
{
	struct kvm_apic_map *map;
	struct kvm_lapic_irq irq = {0};
	int cluster_size = op_64_bit ? 64 : 32;
	int count;

	if (icr & (APIC_DEST_MASK | APIC_SHORT_MASK))
		return -KVM_EINVAL;

	irq.vector = icr & APIC_VECTOR_MASK;
	irq.delivery_mode = icr & APIC_MODE_MASK;
	irq.level = (icr & APIC_INT_ASSERT) != 0;
	irq.trig_mode = icr & APIC_INT_LEVELTRIG;

	rcu_read_lock();
	map = rcu_dereference(kvm->arch.apic_map);

	count = -EOPNOTSUPP;
	if (likely(map)) {
		count = __pv_send_ipi(&ipi_bitmap_low, map, &irq, min);
		min += cluster_size;
		count += __pv_send_ipi(&ipi_bitmap_high, map, &irq, min);
	}

	rcu_read_unlock();
	return count;
}