static void kvm_destroy_dirty_bitmap(struct kvm_memory_slot *memslot)
{
	if (!memslot->dirty_bitmap)
		return;

	if (2 * kvm_dirty_bitmap_bytes(memslot) > PAGE_SIZE)
		vfree(memslot->dirty_bitmap_head);
	else
		kfree(memslot->dirty_bitmap_head);

	memslot->dirty_bitmap = NULL;
	memslot->dirty_bitmap_head = NULL;
}