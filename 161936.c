static int __init efi_update_mem_attr(struct mm_struct *mm, efi_memory_desc_t *md)
{
	unsigned long pf = 0;

	if (md->attribute & EFI_MEMORY_XP)
		pf |= _PAGE_NX;

	if (!(md->attribute & EFI_MEMORY_RO))
		pf |= _PAGE_RW;

	if (sev_active())
		pf |= _PAGE_ENC;

	return efi_update_mappings(md, pf);
}