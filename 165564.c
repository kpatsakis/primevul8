static bool kvm_apic_match_logical_addr(struct kvm_lapic *apic, u32 mda)
{
	u32 logical_id;

	if (kvm_apic_broadcast(apic, mda))
		return true;

	logical_id = kvm_lapic_get_reg(apic, APIC_LDR);

	if (apic_x2apic_mode(apic))
		return ((logical_id >> 16) == (mda >> 16))
		       && (logical_id & mda & 0xffff) != 0;

	logical_id = GET_APIC_LOGICAL_ID(logical_id);

	switch (kvm_lapic_get_reg(apic, APIC_DFR)) {
	case APIC_DFR_FLAT:
		return (logical_id & mda) != 0;
	case APIC_DFR_CLUSTER:
		return ((logical_id >> 4) == (mda >> 4))
		       && (logical_id & mda & 0xf) != 0;
	default:
		return false;
	}
}