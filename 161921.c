efi_thunk_query_capsule_caps(efi_capsule_header_t **capsules,
			     unsigned long count, u64 *max_size,
			     int *reset_type)
{
	/*
	 * To properly support this function we would need to repackage
	 * 'capsules' because the firmware doesn't understand 64-bit
	 * pointers.
	 */
	return EFI_UNSUPPORTED;
}