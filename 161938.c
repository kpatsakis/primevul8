void __init runtime_code_page_mkexec(void)
{
	efi_memory_desc_t *md;

	/* Make EFI runtime service code area executable */
	for_each_efi_memory_desc(md) {
		if (md->type != EFI_RUNTIME_SERVICES_CODE)
			continue;

		efi_set_executable(md, true);
	}
}