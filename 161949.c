static void __init efi_clean_memmap(void)
{
	efi_memory_desc_t *out = efi.memmap.map;
	const efi_memory_desc_t *in = out;
	const efi_memory_desc_t *end = efi.memmap.map_end;
	int i, n_removal;

	for (i = n_removal = 0; in < end; i++) {
		if (efi_memmap_entry_valid(in, i)) {
			if (out != in)
				memcpy(out, in, efi.memmap.desc_size);
			out = (void *)out + efi.memmap.desc_size;
		} else {
			n_removal++;
		}
		in = (void *)in + efi.memmap.desc_size;
	}

	if (n_removal > 0) {
		u64 size = efi.memmap.nr_map - n_removal;

		pr_warn("Removing %d invalid memory map entries.\n", n_removal);
		efi_memmap_install(efi.memmap.phys_map, size);
	}
}