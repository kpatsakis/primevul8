static void __init do_add_efi_memmap(void)
{
	efi_memory_desc_t *md;

	for_each_efi_memory_desc(md) {
		unsigned long long start = md->phys_addr;
		unsigned long long size = md->num_pages << EFI_PAGE_SHIFT;
		int e820_type;

		switch (md->type) {
		case EFI_LOADER_CODE:
		case EFI_LOADER_DATA:
		case EFI_BOOT_SERVICES_CODE:
		case EFI_BOOT_SERVICES_DATA:
		case EFI_CONVENTIONAL_MEMORY:
			if (md->attribute & EFI_MEMORY_WB)
				e820_type = E820_TYPE_RAM;
			else
				e820_type = E820_TYPE_RESERVED;
			break;
		case EFI_ACPI_RECLAIM_MEMORY:
			e820_type = E820_TYPE_ACPI;
			break;
		case EFI_ACPI_MEMORY_NVS:
			e820_type = E820_TYPE_NVS;
			break;
		case EFI_UNUSABLE_MEMORY:
			e820_type = E820_TYPE_UNUSABLE;
			break;
		case EFI_PERSISTENT_MEMORY:
			e820_type = E820_TYPE_PMEM;
			break;
		default:
			/*
			 * EFI_RESERVED_TYPE EFI_RUNTIME_SERVICES_CODE
			 * EFI_RUNTIME_SERVICES_DATA EFI_MEMORY_MAPPED_IO
			 * EFI_MEMORY_MAPPED_IO_PORT_SPACE EFI_PAL_CODE
			 */
			e820_type = E820_TYPE_RESERVED;
			break;
		}
		e820__range_add(start, size, e820_type);
	}
	e820__update_table(e820_table);
}