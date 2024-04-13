void __init efi_init(void)
{
	efi_char16_t *c16;
	char vendor[100] = "unknown";
	int i = 0;
	void *tmp;

#ifdef CONFIG_X86_32
	if (boot_params.efi_info.efi_systab_hi ||
	    boot_params.efi_info.efi_memmap_hi) {
		pr_info("Table located above 4GB, disabling EFI.\n");
		return;
	}
	efi_phys.systab = (efi_system_table_t *)boot_params.efi_info.efi_systab;
#else
	efi_phys.systab = (efi_system_table_t *)
			  (boot_params.efi_info.efi_systab |
			  ((__u64)boot_params.efi_info.efi_systab_hi<<32));
#endif

	if (efi_systab_init(efi_phys.systab))
		return;

	efi.config_table = (unsigned long)efi.systab->tables;
	efi.fw_vendor	 = (unsigned long)efi.systab->fw_vendor;
	efi.runtime	 = (unsigned long)efi.systab->runtime;

	/*
	 * Show what we know for posterity
	 */
	c16 = tmp = early_memremap(efi.systab->fw_vendor, 2);
	if (c16) {
		for (i = 0; i < sizeof(vendor) - 1 && *c16; ++i)
			vendor[i] = *c16++;
		vendor[i] = '\0';
	} else
		pr_err("Could not map the firmware vendor!\n");
	early_memunmap(tmp, 2);

	pr_info("EFI v%u.%.02u by %s\n",
		efi.systab->hdr.revision >> 16,
		efi.systab->hdr.revision & 0xffff, vendor);

	if (efi_reuse_config(efi.systab->tables, efi.systab->nr_tables))
		return;

	if (efi_config_init(arch_tables))
		return;

	/*
	 * Note: We currently don't support runtime services on an EFI
	 * that doesn't match the kernel 32/64-bit mode.
	 */

	if (!efi_runtime_supported())
		pr_info("No EFI runtime due to 32/64-bit mismatch with kernel\n");
	else {
		if (efi_runtime_disabled() || efi_runtime_init()) {
			efi_memmap_unmap();
			return;
		}
	}

	efi_clean_memmap();

	if (efi_enabled(EFI_DBG))
		efi_print_memmap();
}