void __init old_map_region(efi_memory_desc_t *md)
{
	u64 start_pfn, end_pfn, end;
	unsigned long size;
	void *va;

	start_pfn = PFN_DOWN(md->phys_addr);
	size	  = md->num_pages << PAGE_SHIFT;
	end	  = md->phys_addr + size;
	end_pfn   = PFN_UP(end);

	if (pfn_range_is_mapped(start_pfn, end_pfn)) {
		va = __va(md->phys_addr);

		if (!(md->attribute & EFI_MEMORY_WB))
			efi_memory_uc((u64)(unsigned long)va, size);
	} else
		va = efi_ioremap(md->phys_addr, size,
				 md->type, md->attribute);

	md->virt_addr = (u64) (unsigned long) va;
	if (!va)
		pr_err("ioremap of 0x%llX failed!\n",
		       (unsigned long long)md->phys_addr);
}