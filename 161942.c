efi_thunk_get_next_high_mono_count(u32 *count)
{
	efi_status_t status;
	u32 phys_count;
	unsigned long flags;

	spin_lock_irqsave(&efi_runtime_lock, flags);

	phys_count = virt_to_phys_or_null(count);
	status = efi_thunk(get_next_high_mono_count, phys_count);

	spin_unlock_irqrestore(&efi_runtime_lock, flags);

	return status;
}