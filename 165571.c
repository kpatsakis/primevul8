static u32 __apic_read(struct kvm_lapic *apic, unsigned int offset)
{
	u32 val = 0;

	if (offset >= LAPIC_MMIO_LENGTH)
		return 0;

	switch (offset) {
	case APIC_ARBPRI:
		break;

	case APIC_TMCCT:	/* Timer CCR */
		if (apic_lvtt_tscdeadline(apic))
			return 0;

		val = apic_get_tmcct(apic);
		break;
	case APIC_PROCPRI:
		apic_update_ppr(apic);
		val = kvm_lapic_get_reg(apic, offset);
		break;
	case APIC_TASKPRI:
		report_tpr_access(apic, false);
		fallthrough;
	default:
		val = kvm_lapic_get_reg(apic, offset);
		break;
	}

	return val;
}