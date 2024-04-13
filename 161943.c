static void __init get_systab_virt_addr(efi_memory_desc_t *md)
{
	unsigned long size;
	u64 end, systab;

	size = md->num_pages << EFI_PAGE_SHIFT;
	end = md->phys_addr + size;
	systab = (u64)(unsigned long)efi_phys.systab;
	if (md->phys_addr <= systab && systab < end) {
		systab += md->virt_addr - md->phys_addr;
		efi.systab = (efi_system_table_t *)(unsigned long)systab;
	}
}