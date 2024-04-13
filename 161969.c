static void *efi_map_next_entry(void *entry)
{
	if (!efi_enabled(EFI_OLD_MEMMAP) && efi_enabled(EFI_64BIT)) {
		/*
		 * Starting in UEFI v2.5 the EFI_PROPERTIES_TABLE
		 * config table feature requires us to map all entries
		 * in the same order as they appear in the EFI memory
		 * map. That is to say, entry N must have a lower
		 * virtual address than entry N+1. This is because the
		 * firmware toolchain leaves relative references in
		 * the code/data sections, which are split and become
		 * separate EFI memory regions. Mapping things
		 * out-of-order leads to the firmware accessing
		 * unmapped addresses.
		 *
		 * Since we need to map things this way whether or not
		 * the kernel actually makes use of
		 * EFI_PROPERTIES_TABLE, let's just switch to this
		 * scheme by default for 64-bit.
		 */
		return efi_map_next_entry_reverse(entry);
	}

	/* Initial call */
	if (!entry)
		return efi.memmap.map;

	entry += efi.memmap.desc_size;
	if (entry >= efi.memmap.map_end)
		return NULL;

	return entry;
}