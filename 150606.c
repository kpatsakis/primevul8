static unsigned long gfn_to_hva_many(struct kvm_memory_slot *slot, gfn_t gfn,
				     gfn_t *nr_pages)
{
	if (!slot || slot->flags & KVM_MEMSLOT_INVALID)
		return bad_hva();

	if (nr_pages)
		*nr_pages = slot->npages - (gfn - slot->base_gfn);

	return gfn_to_hva_memslot(slot, gfn);
}