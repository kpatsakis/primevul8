efi_thunk_get_next_variable(unsigned long *name_size,
			    efi_char16_t *name,
			    efi_guid_t *vendor)
{
	efi_status_t status;
	u32 phys_name_size, phys_name, phys_vendor;
	unsigned long flags;

	spin_lock_irqsave(&efi_runtime_lock, flags);

	phys_name_size = virt_to_phys_or_null(name_size);
	phys_vendor = virt_to_phys_or_null(vendor);
	phys_name = virt_to_phys_or_null_size(name, *name_size);

	status = efi_thunk(get_next_variable, phys_name_size,
			   phys_name, phys_vendor);

	spin_unlock_irqrestore(&efi_runtime_lock, flags);

	return status;
}