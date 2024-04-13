static bool __init efi_memmap_entry_valid(const efi_memory_desc_t *md, int i)
{
	u64 end = (md->num_pages << EFI_PAGE_SHIFT) + md->phys_addr - 1;
	u64 end_hi = 0;
	char buf[64];

	if (md->num_pages == 0) {
		end = 0;
	} else if (md->num_pages > EFI_PAGES_MAX ||
		   EFI_PAGES_MAX - md->num_pages <
		   (md->phys_addr >> EFI_PAGE_SHIFT)) {
		end_hi = (md->num_pages & OVERFLOW_ADDR_MASK)
			>> OVERFLOW_ADDR_SHIFT;

		if ((md->phys_addr & U64_HIGH_BIT) && !(end & U64_HIGH_BIT))
			end_hi += 1;
	} else {
		return true;
	}

	pr_warn_once(FW_BUG "Invalid EFI memory map entries:\n");

	if (end_hi) {
		pr_warn("mem%02u: %s range=[0x%016llx-0x%llx%016llx] (invalid)\n",
			i, efi_md_typeattr_format(buf, sizeof(buf), md),
			md->phys_addr, end_hi, end);
	} else {
		pr_warn("mem%02u: %s range=[0x%016llx-0x%016llx] (invalid)\n",
			i, efi_md_typeattr_format(buf, sizeof(buf), md),
			md->phys_addr, end);
	}
	return false;
}