void kvm_release_page_dirty(struct page *page)
{
	kvm_release_pfn_dirty(page_to_pfn(page));
}