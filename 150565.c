int gfn_to_page_many_atomic(struct kvm *kvm, gfn_t gfn, struct page **pages,
								  int nr_pages)
{
	unsigned long addr;
	gfn_t entry;

	addr = gfn_to_hva_many(gfn_to_memslot(kvm, gfn), gfn, &entry);
	if (kvm_is_error_hva(addr))
		return -1;

	if (entry < nr_pages)
		return 0;

	return __get_user_pages_fast(addr, nr_pages, 1, pages);
}