void __init efi_find_mirror(void)
{
	efi_memory_desc_t *md;
	u64 mirror_size = 0, total_size = 0;

	for_each_efi_memory_desc(md) {
		unsigned long long start = md->phys_addr;
		unsigned long long size = md->num_pages << EFI_PAGE_SHIFT;

		total_size += size;
		if (md->attribute & EFI_MEMORY_MORE_RELIABLE) {
			memblock_mark_mirror(start, size);
			mirror_size += size;
		}
	}
	if (mirror_size)
		pr_info("Memory: %lldM/%lldM mirrored memory\n",
			mirror_size>>20, total_size>>20);
}