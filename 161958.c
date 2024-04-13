efi_thunk_set_variable_nonblocking(efi_char16_t *name, efi_guid_t *vendor,
				   u32 attr, unsigned long data_size,
				   void *data)
{
	u32 phys_name, phys_vendor, phys_data;
	efi_status_t status;
	unsigned long flags;

	if (!spin_trylock_irqsave(&efi_runtime_lock, flags))
		return EFI_NOT_READY;

	phys_name = virt_to_phys_or_null_size(name, efi_name_size(name));
	phys_vendor = virt_to_phys_or_null(vendor);
	phys_data = virt_to_phys_or_null_size(data, data_size);

	/* If data_size is > sizeof(u32) we've got problems */
	status = efi_thunk(set_variable, phys_name, phys_vendor,
			   attr, data_size, phys_data);

	spin_unlock_irqrestore(&efi_runtime_lock, flags);

	return status;
}