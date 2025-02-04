static efi_status_t __init phys_efi_set_virtual_address_map(
	unsigned long memory_map_size,
	unsigned long descriptor_size,
	u32 descriptor_version,
	efi_memory_desc_t *virtual_map)
{
	efi_status_t status;
	unsigned long flags;
	pgd_t *save_pgd;

	save_pgd = efi_call_phys_prolog();
	if (!save_pgd)
		return EFI_ABORTED;

	/* Disable interrupts around EFI calls: */
	local_irq_save(flags);
	status = efi_call_phys(efi_phys.set_virtual_address_map,
			       memory_map_size, descriptor_size,
			       descriptor_version, virtual_map);
	local_irq_restore(flags);

	efi_call_phys_epilog(save_pgd);

	return status;
}