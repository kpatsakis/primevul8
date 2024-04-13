static int __init efi_runtime_init(void)
{
	int rv;

	/*
	 * Check out the runtime services table. We need to map
	 * the runtime services table so that we can grab the physical
	 * address of several of the EFI runtime functions, needed to
	 * set the firmware into virtual mode.
	 *
	 * When EFI_PARAVIRT is in force then we could not map runtime
	 * service memory region because we do not have direct access to it.
	 * However, runtime services are available through proxy functions
	 * (e.g. in case of Xen dom0 EFI implementation they call special
	 * hypercall which executes relevant EFI functions) and that is why
	 * they are always enabled.
	 */

	if (!efi_enabled(EFI_PARAVIRT)) {
		if (efi_enabled(EFI_64BIT))
			rv = efi_runtime_init64();
		else
			rv = efi_runtime_init32();

		if (rv)
			return rv;
	}

	set_bit(EFI_RUNTIME_SERVICES, &efi.flags);

	return 0;
}