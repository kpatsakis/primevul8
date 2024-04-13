int __init efi_memblock_x86_reserve_range(void)
{
	struct efi_info *e = &boot_params.efi_info;
	struct efi_memory_map_data data;
	phys_addr_t pmap;
	int rv;

	if (efi_enabled(EFI_PARAVIRT))
		return 0;

#ifdef CONFIG_X86_32
	/* Can't handle data above 4GB at this time */
	if (e->efi_memmap_hi) {
		pr_err("Memory map is above 4GB, disabling EFI.\n");
		return -EINVAL;
	}
	pmap =  e->efi_memmap;
#else
	pmap = (e->efi_memmap |	((__u64)e->efi_memmap_hi << 32));
#endif
	data.phys_map		= pmap;
	data.size 		= e->efi_memmap_size;
	data.desc_size		= e->efi_memdesc_size;
	data.desc_version	= e->efi_memdesc_version;

	rv = efi_memmap_init_early(&data);
	if (rv)
		return rv;

	if (add_efi_memmap)
		do_add_efi_memmap();

	WARN(efi.memmap.desc_version != 1,
	     "Unexpected EFI_MEMORY_DESCRIPTOR version %ld",
	     efi.memmap.desc_version);

	memblock_reserve(pmap, efi.memmap.nr_map * efi.memmap.desc_size);

	return 0;
}