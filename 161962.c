void __init efi_map_region_fixed(efi_memory_desc_t *md)
{
	__map_region(md, md->phys_addr);
	__map_region(md, md->virt_addr);
}