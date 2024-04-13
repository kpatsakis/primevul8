efi_status_t efi_thunk_set_virtual_address_map(
	void *phys_set_virtual_address_map,
	unsigned long memory_map_size,
	unsigned long descriptor_size,
	u32 descriptor_version,
	efi_memory_desc_t *virtual_map)
{
	efi_status_t status;
	unsigned long flags;
	u32 func;

	efi_sync_low_kernel_mappings();
	local_irq_save(flags);

	efi_switch_mm(&efi_mm);

	func = (u32)(unsigned long)phys_set_virtual_address_map;
	status = efi64_thunk(func, memory_map_size, descriptor_size,
			     descriptor_version, virtual_map);

	efi_switch_mm(efi_scratch.prev_mm);
	local_irq_restore(flags);

	return status;
}