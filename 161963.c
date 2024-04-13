static void * __init efi_map_regions(int *count, int *pg_shift)
{
	void *p, *new_memmap = NULL;
	unsigned long left = 0;
	unsigned long desc_size;
	efi_memory_desc_t *md;

	desc_size = efi.memmap.desc_size;

	p = NULL;
	while ((p = efi_map_next_entry(p))) {
		md = p;

		if (!should_map_region(md))
			continue;

		efi_map_region(md);
		get_systab_virt_addr(md);

		if (left < desc_size) {
			new_memmap = realloc_pages(new_memmap, *pg_shift);
			if (!new_memmap)
				return NULL;

			left += PAGE_SIZE << *pg_shift;
			(*pg_shift)++;
		}

		memcpy(new_memmap + (*count * desc_size), md, desc_size);

		left -= desc_size;
		(*count)++;
	}

	return new_memmap;
}