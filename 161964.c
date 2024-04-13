void __init efi_runtime_update_mappings(void)
{
	efi_memory_desc_t *md;

	if (efi_enabled(EFI_OLD_MEMMAP)) {
		if (__supported_pte_mask & _PAGE_NX)
			runtime_code_page_mkexec();
		return;
	}

	/*
	 * Use the EFI Memory Attribute Table for mapping permissions if it
	 * exists, since it is intended to supersede EFI_PROPERTIES_TABLE.
	 */
	if (efi_enabled(EFI_MEM_ATTR)) {
		efi_memattr_apply_permissions(NULL, efi_update_mem_attr);
		return;
	}

	/*
	 * EFI_MEMORY_ATTRIBUTES_TABLE is intended to replace
	 * EFI_PROPERTIES_TABLE. So, use EFI_PROPERTIES_TABLE to update
	 * permissions only if EFI_MEMORY_ATTRIBUTES_TABLE is not
	 * published by the firmware. Even if we find a buggy implementation of
	 * EFI_MEMORY_ATTRIBUTES_TABLE, don't fall back to
	 * EFI_PROPERTIES_TABLE, because of the same reason.
	 */

	if (!efi_enabled(EFI_NX_PE_DATA))
		return;

	for_each_efi_memory_desc(md) {
		unsigned long pf = 0;

		if (!(md->attribute & EFI_MEMORY_RUNTIME))
			continue;

		if (!(md->attribute & EFI_MEMORY_WB))
			pf |= _PAGE_PCD;

		if ((md->attribute & EFI_MEMORY_XP) ||
			(md->type == EFI_RUNTIME_SERVICES_DATA))
			pf |= _PAGE_NX;

		if (!(md->attribute & EFI_MEMORY_RO) &&
			(md->type != EFI_RUNTIME_SERVICES_CODE))
			pf |= _PAGE_RW;

		if (sev_active())
			pf |= _PAGE_ENC;

		efi_update_mappings(md, pf);
	}
}