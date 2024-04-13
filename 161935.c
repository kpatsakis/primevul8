efi_thunk_update_capsule(efi_capsule_header_t **capsules,
			 unsigned long count, unsigned long sg_list)
{
	/*
	 * To properly support this function we would need to repackage
	 * 'capsules' because the firmware doesn't understand 64-bit
	 * pointers.
	 */
	return EFI_UNSUPPORTED;
}