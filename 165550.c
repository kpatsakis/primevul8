static inline bool kvm_apic_map_get_dest_lapic(struct kvm *kvm,
		struct kvm_lapic **src, struct kvm_lapic_irq *irq,
		struct kvm_apic_map *map, struct kvm_lapic ***dst,
		unsigned long *bitmap)
{
	int i, lowest;

	if (irq->shorthand == APIC_DEST_SELF && src) {
		*dst = src;
		*bitmap = 1;
		return true;
	} else if (irq->shorthand)
		return false;

	if (!map || kvm_apic_is_broadcast_dest(kvm, src, irq, map))
		return false;

	if (irq->dest_mode == APIC_DEST_PHYSICAL) {
		if (irq->dest_id > map->max_apic_id) {
			*bitmap = 0;
		} else {
			u32 dest_id = array_index_nospec(irq->dest_id, map->max_apic_id + 1);
			*dst = &map->phys_map[dest_id];
			*bitmap = 1;
		}
		return true;
	}

	*bitmap = 0;
	if (!kvm_apic_map_get_logical_dest(map, irq->dest_id, dst,
				(u16 *)bitmap))
		return false;

	if (!kvm_lowest_prio_delivery(irq))
		return true;

	if (!kvm_vector_hashing_enabled()) {
		lowest = -1;
		for_each_set_bit(i, bitmap, 16) {
			if (!(*dst)[i])
				continue;
			if (lowest < 0)
				lowest = i;
			else if (kvm_apic_compare_prio((*dst)[i]->vcpu,
						(*dst)[lowest]->vcpu) < 0)
				lowest = i;
		}
	} else {
		if (!*bitmap)
			return true;

		lowest = kvm_vector_to_index(irq->vector, hweight16(*bitmap),
				bitmap, 16);

		if (!(*dst)[lowest]) {
			kvm_apic_disabled_lapic_found(kvm);
			*bitmap = 0;
			return true;
		}
	}

	*bitmap = (lowest >= 0) ? 1 << lowest : 0;

	return true;
}