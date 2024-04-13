efi_thunk_query_variable_info_nonblocking(u32 attr, u64 *storage_space,
					  u64 *remaining_space,
					  u64 *max_variable_size)
{
	efi_status_t status;
	u32 phys_storage, phys_remaining, phys_max;
	unsigned long flags;

	if (efi.runtime_version < EFI_2_00_SYSTEM_TABLE_REVISION)
		return EFI_UNSUPPORTED;

	if (!spin_trylock_irqsave(&efi_runtime_lock, flags))
		return EFI_NOT_READY;

	phys_storage = virt_to_phys_or_null(storage_space);
	phys_remaining = virt_to_phys_or_null(remaining_space);
	phys_max = virt_to_phys_or_null(max_variable_size);

	status = efi_thunk(query_variable_info, attr, phys_storage,
			   phys_remaining, phys_max);

	spin_unlock_irqrestore(&efi_runtime_lock, flags);

	return status;
}