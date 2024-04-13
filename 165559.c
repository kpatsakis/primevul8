static int apic_mmio_write(struct kvm_vcpu *vcpu, struct kvm_io_device *this,
			    gpa_t address, int len, const void *data)
{
	struct kvm_lapic *apic = to_lapic(this);
	unsigned int offset = address - apic->base_address;
	u32 val;

	if (!apic_mmio_in_range(apic, address))
		return -EOPNOTSUPP;

	if (!kvm_apic_hw_enabled(apic) || apic_x2apic_mode(apic)) {
		if (!kvm_check_has_quirk(vcpu->kvm,
					 KVM_X86_QUIRK_LAPIC_MMIO_HOLE))
			return -EOPNOTSUPP;

		return 0;
	}

	/*
	 * APIC register must be aligned on 128-bits boundary.
	 * 32/64/128 bits registers must be accessed thru 32 bits.
	 * Refer SDM 8.4.1
	 */
	if (len != 4 || (offset & 0xf))
		return 0;

	val = *(u32*)data;

	kvm_lapic_reg_write(apic, offset & 0xff0, val);

	return 0;
}