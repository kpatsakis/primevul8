efi_thunk_get_variable(efi_char16_t *name, efi_guid_t *vendor,
		       u32 *attr, unsigned long *data_size, void *data)
{
	efi_status_t status;
	u32 phys_name, phys_vendor, phys_attr;
	u32 phys_data_size, phys_data;
	unsigned long flags;

	spin_lock_irqsave(&efi_runtime_lock, flags);

	phys_data_size = virt_to_phys_or_null(data_size);
	phys_vendor = virt_to_phys_or_null(vendor);
	phys_name = virt_to_phys_or_null_size(name, efi_name_size(name));
	phys_attr = virt_to_phys_or_null(attr);
	phys_data = virt_to_phys_or_null_size(data, *data_size);

	status = efi_thunk(get_variable, phys_name, phys_vendor,
			   phys_attr, phys_data_size, phys_data);

	spin_unlock_irqrestore(&efi_runtime_lock, flags);

	return status;
}