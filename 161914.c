efi_thunk_reset_system(int reset_type, efi_status_t status,
		       unsigned long data_size, efi_char16_t *data)
{
	u32 phys_data;
	unsigned long flags;

	spin_lock_irqsave(&efi_runtime_lock, flags);

	phys_data = virt_to_phys_or_null_size(data, data_size);

	efi_thunk(reset_system, reset_type, status, data_size, phys_data);

	spin_unlock_irqrestore(&efi_runtime_lock, flags);
}