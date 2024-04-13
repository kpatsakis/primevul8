void __iomem *__init efi_ioremap(unsigned long phys_addr, unsigned long size,
				 u32 type, u64 attribute)
{
	unsigned long last_map_pfn;

	if (type == EFI_MEMORY_MAPPED_IO)
		return ioremap(phys_addr, size);

	last_map_pfn = init_memory_mapping(phys_addr, phys_addr + size);
	if ((last_map_pfn << PAGE_SHIFT) < phys_addr + size) {
		unsigned long top = last_map_pfn << PAGE_SHIFT;
		efi_ioremap(top, size - (top - phys_addr), type, attribute);
	}

	if (!(attribute & EFI_MEMORY_WB))
		efi_memory_uc((u64)(unsigned long)__va(phys_addr), size);

	return (void __iomem *)__va(phys_addr);
}