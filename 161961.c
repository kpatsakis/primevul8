static int __init efi_runtime_init64(void)
{
	efi_runtime_services_64_t *runtime;

	runtime = early_memremap((unsigned long)efi.systab->runtime,
			sizeof(efi_runtime_services_64_t));
	if (!runtime) {
		pr_err("Could not map the runtime service table!\n");
		return -ENOMEM;
	}

	/*
	 * We will only need *early* access to the SetVirtualAddressMap
	 * EFI runtime service. All other runtime services will be called
	 * via the virtual mapping.
	 */
	efi_phys.set_virtual_address_map =
			(efi_set_virtual_address_map_t *)
			(unsigned long)runtime->set_virtual_address_map;
	early_memunmap(runtime, sizeof(efi_runtime_services_64_t));

	return 0;
}