void kvm_set_pfn_accessed(pfn_t pfn)
{
	if (!kvm_is_mmio_pfn(pfn))
		mark_page_accessed(pfn_to_page(pfn));
}