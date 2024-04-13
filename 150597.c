void kvm_release_page_clean(struct page *page)
{
	kvm_release_pfn_clean(page_to_pfn(page));
}