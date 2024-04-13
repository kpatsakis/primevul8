int kvm_gfn_to_hva_cache_init(struct kvm *kvm, struct gfn_to_hva_cache *ghc,
			      gpa_t gpa)
{
	struct kvm_memslots *slots = kvm_memslots(kvm);
	int offset = offset_in_page(gpa);
	gfn_t gfn = gpa >> PAGE_SHIFT;

	ghc->gpa = gpa;
	ghc->generation = slots->generation;
	ghc->memslot = __gfn_to_memslot(slots, gfn);
	ghc->hva = gfn_to_hva_many(ghc->memslot, gfn, NULL);
	if (!kvm_is_error_hva(ghc->hva))
		ghc->hva += offset;
	else
		return -EFAULT;

	return 0;
}