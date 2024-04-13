static void __init __map_region(efi_memory_desc_t *md, u64 va)
{
	unsigned long flags = _PAGE_RW;
	unsigned long pfn;
	pgd_t *pgd = efi_mm.pgd;

	if (!(md->attribute & EFI_MEMORY_WB))
		flags |= _PAGE_PCD;

	if (sev_active() && md->type != EFI_MEMORY_MAPPED_IO)
		flags |= _PAGE_ENC;

	pfn = md->phys_addr >> PAGE_SHIFT;
	if (kernel_map_pages_in_pgd(pgd, pfn, va, md->num_pages, flags))
		pr_warn("Error mapping PA 0x%llx -> VA 0x%llx!\n",
			   md->phys_addr, va);
}