static void update_divide_count(struct kvm_lapic *apic)
{
	u32 tmp1, tmp2, tdcr;

	tdcr = kvm_lapic_get_reg(apic, APIC_TDCR);
	tmp1 = tdcr & 0xf;
	tmp2 = ((tmp1 & 0x3) | ((tmp1 & 0x8) >> 1)) + 1;
	apic->divide_count = 0x1 << (tmp2 & 0x7);
}