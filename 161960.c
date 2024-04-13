static void __init early_code_mapping_set_exec(int executable)
{
	efi_memory_desc_t *md;

	if (!(__supported_pte_mask & _PAGE_NX))
		return;

	/* Make EFI service code area executable */
	for_each_efi_memory_desc(md) {
		if (md->type == EFI_RUNTIME_SERVICES_CODE ||
		    md->type == EFI_BOOT_SERVICES_CODE)
			efi_set_executable(md, executable);
	}
}