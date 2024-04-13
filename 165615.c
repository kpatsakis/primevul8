static int __pv_send_ipi(unsigned long *ipi_bitmap, struct kvm_apic_map *map,
			 struct kvm_lapic_irq *irq, u32 min)
{
	int i, count = 0;
	struct kvm_vcpu *vcpu;

	if (min > map->max_apic_id)
		return 0;

	for_each_set_bit(i, ipi_bitmap,
		min((u32)BITS_PER_LONG, (map->max_apic_id - min + 1))) {
		if (map->phys_map[min + i]) {
			vcpu = map->phys_map[min + i]->vcpu;
			count += kvm_apic_set_irq(vcpu, irq, NULL);
		}
	}

	return count;
}