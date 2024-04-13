static inline bool kvm_apic_map_get_logical_dest(struct kvm_apic_map *map,
		u32 dest_id, struct kvm_lapic ***cluster, u16 *mask) {
	switch (map->mode) {
	case KVM_APIC_MODE_X2APIC: {
		u32 offset = (dest_id >> 16) * 16;
		u32 max_apic_id = map->max_apic_id;

		if (offset <= max_apic_id) {
			u8 cluster_size = min(max_apic_id - offset + 1, 16U);

			offset = array_index_nospec(offset, map->max_apic_id + 1);
			*cluster = &map->phys_map[offset];
			*mask = dest_id & (0xffff >> (16 - cluster_size));
		} else {
			*mask = 0;
		}

		return true;
		}
	case KVM_APIC_MODE_XAPIC_FLAT:
		*cluster = map->xapic_flat_map;
		*mask = dest_id & 0xff;
		return true;
	case KVM_APIC_MODE_XAPIC_CLUSTER:
		*cluster = map->xapic_cluster_map[(dest_id >> 4) & 0xf];
		*mask = dest_id & 0xf;
		return true;
	default:
		/* Not optimized. */
		return false;
	}
}