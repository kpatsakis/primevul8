static void __init __efi_enter_virtual_mode(void)
{
	int count = 0, pg_shift = 0;
	void *new_memmap = NULL;
	efi_status_t status;
	unsigned long pa;

	efi.systab = NULL;

	if (efi_alloc_page_tables()) {
		pr_err("Failed to allocate EFI page tables\n");
		clear_bit(EFI_RUNTIME_SERVICES, &efi.flags);
		return;
	}

	efi_merge_regions();
	new_memmap = efi_map_regions(&count, &pg_shift);
	if (!new_memmap) {
		pr_err("Error reallocating memory, EFI runtime non-functional!\n");
		clear_bit(EFI_RUNTIME_SERVICES, &efi.flags);
		return;
	}

	pa = __pa(new_memmap);

	/*
	 * Unregister the early EFI memmap from efi_init() and install
	 * the new EFI memory map that we are about to pass to the
	 * firmware via SetVirtualAddressMap().
	 */
	efi_memmap_unmap();

	if (efi_memmap_init_late(pa, efi.memmap.desc_size * count)) {
		pr_err("Failed to remap late EFI memory map\n");
		clear_bit(EFI_RUNTIME_SERVICES, &efi.flags);
		return;
	}

	if (efi_enabled(EFI_DBG)) {
		pr_info("EFI runtime memory map:\n");
		efi_print_memmap();
	}

	BUG_ON(!efi.systab);

	if (efi_setup_page_tables(pa, 1 << pg_shift)) {
		clear_bit(EFI_RUNTIME_SERVICES, &efi.flags);
		return;
	}

	efi_sync_low_kernel_mappings();

	if (efi_is_native()) {
		status = phys_efi_set_virtual_address_map(
				efi.memmap.desc_size * count,
				efi.memmap.desc_size,
				efi.memmap.desc_version,
				(efi_memory_desc_t *)pa);
	} else {
		status = efi_thunk_set_virtual_address_map(
				efi_phys.set_virtual_address_map,
				efi.memmap.desc_size * count,
				efi.memmap.desc_size,
				efi.memmap.desc_version,
				(efi_memory_desc_t *)pa);
	}

	if (status != EFI_SUCCESS) {
		pr_alert("Unable to switch EFI into virtual mode (status=%lx)!\n",
			 status);
		panic("EFI call to SetVirtualAddressMap() failed!");
	}

	efi_free_boot_services();

	/*
	 * Now that EFI is in virtual mode, update the function
	 * pointers in the runtime service table to the new virtual addresses.
	 *
	 * Call EFI services through wrapper functions.
	 */
	efi.runtime_version = efi_systab.hdr.revision;

	if (efi_is_native())
		efi_native_runtime_setup();
	else
		efi_thunk_runtime_setup();

	efi.set_virtual_address_map = NULL;

	/*
	 * Apply more restrictive page table mapping attributes now that
	 * SVAM() has been called and the firmware has performed all
	 * necessary relocation fixups for the new virtual addresses.
	 */
	efi_runtime_update_mappings();

	/* clean DUMMY object */
	efi_delete_dummy_variable();
}