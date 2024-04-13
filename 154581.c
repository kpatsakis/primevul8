static void vmx_dump_dtsel(char *name, uint32_t limit)
{
	pr_err("%s                           limit=0x%08x, base=0x%016lx\n",
	       name, vmcs_read32(limit),
	       vmcs_readl(limit + GUEST_GDTR_BASE - GUEST_GDTR_LIMIT));
}