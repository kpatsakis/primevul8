void __init efi_enter_virtual_mode(void)
{
	if (efi_enabled(EFI_PARAVIRT))
		return;

	if (efi_setup)
		kexec_enter_virtual_mode();
	else
		__efi_enter_virtual_mode();

	efi_dump_pagetable();
}