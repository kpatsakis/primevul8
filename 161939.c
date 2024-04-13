void __init efi_memory_uc(u64 addr, unsigned long size)
{
	unsigned long page_shift = 1UL << EFI_PAGE_SHIFT;
	u64 npages;

	npages = round_up(size, page_shift) / page_shift;
	memrange_efi_to_native(&addr, &npages);
	set_memory_uc(addr, npages);
}