static void __init kexec_enter_virtual_mode(void)
{
#ifdef CONFIG_KEXEC_CORE
	efi_memory_desc_t *md;
	unsigned int num_pages;

	efi.systab = NULL;

	/*
	 * We don't do virtual mode, since we don't do runtime services, on
	 * non-native EFI. With efi=old_map, we don't do runtime services in
	 * kexec kernel because in the initial boot something else might
	 * have been mapped at these virtual addresses.
	 */
	if (!efi_is_native() || efi_enabled(EFI_OLD_MEMMAP)) {
		efi_memmap_unmap();
		clear_bit(EFI_RUNTIME_SERVICES, &efi.flags);
		return;
	}

	if (efi_alloc_page_tables()) {
		pr_err("Failed to allocate EFI page tables\n");
		clear_bit(EFI_RUNTIME_SERVICES, &efi.flags);
		return;
	}

	/*
	* Map efi regions which were passed via setup_data. The virt_addr is a
	* fixed addr which was used in first kernel of a kexec boot.
	*/
	for_each_efi_memory_desc(md) {
		efi_map_region_fixed(md); /* FIXME: add error handling */
		get_systab_virt_addr(md);
	}

	/*
	 * Unregister the early EFI memmap from efi_init() and install
	 * the new EFI memory map.
	 */
	efi_memmap_unmap();

	if (efi_memmap_init_late(efi.memmap.phys_map,
				 efi.memmap.desc_size * efi.memmap.nr_map)) {
		pr_err("Failed to remap late EFI memory map\n");
		clear_bit(EFI_RUNTIME_SERVICES, &efi.flags);
		return;
	}

	BUG_ON(!efi.systab);

	num_pages = ALIGN(efi.memmap.nr_map * efi.memmap.desc_size, PAGE_SIZE);
	num_pages >>= PAGE_SHIFT;

	if (efi_setup_page_tables(efi.memmap.phys_map, num_pages)) {
		clear_bit(EFI_RUNTIME_SERVICES, &efi.flags);
		return;
	}

	efi_sync_low_kernel_mappings();

	/*
	 * Now that EFI is in virtual mode, update the function
	 * pointers in the runtime service table to the new virtual addresses.
	 *
	 * Call EFI services through wrapper functions.
	 */
	efi.runtime_version = efi_systab.hdr.revision;

	efi_native_runtime_setup();

	efi.set_virtual_address_map = NULL;

	if (efi_enabled(EFI_OLD_MEMMAP) && (__supported_pte_mask & _PAGE_NX))
		runtime_code_page_mkexec();

	/* clean DUMMY object */
	efi_delete_dummy_variable();
#endif
}