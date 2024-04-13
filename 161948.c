void __init efi_set_executable(efi_memory_desc_t *md, bool executable)
{
	u64 addr, npages;

	addr = md->virt_addr;
	npages = md->num_pages;

	memrange_efi_to_native(&addr, &npages);

	if (executable)
		set_memory_x(addr, npages);
	else
		set_memory_nx(addr, npages);
}