static pfn_t hva_to_pfn(struct kvm *kvm, unsigned long addr, bool atomic,
			bool *async, bool write_fault, bool *writable)
{
	struct page *page[1];
	int npages = 0;
	pfn_t pfn;

	/* we can do it either atomically or asynchronously, not both */
	BUG_ON(atomic && async);

	BUG_ON(!write_fault && !writable);

	if (writable)
		*writable = true;

	if (atomic || async)
		npages = __get_user_pages_fast(addr, 1, 1, page);

	if (unlikely(npages != 1) && !atomic) {
		might_sleep();

		if (writable)
			*writable = write_fault;

		if (async) {
			down_read(&current->mm->mmap_sem);
			npages = get_user_page_nowait(current, current->mm,
						     addr, write_fault, page);
			up_read(&current->mm->mmap_sem);
		} else
			npages = get_user_pages_fast(addr, 1, write_fault,
						     page);

		/* map read fault as writable if possible */
		if (unlikely(!write_fault) && npages == 1) {
			struct page *wpage[1];

			npages = __get_user_pages_fast(addr, 1, 1, wpage);
			if (npages == 1) {
				*writable = true;
				put_page(page[0]);
				page[0] = wpage[0];
			}
			npages = 1;
		}
	}

	if (unlikely(npages != 1)) {
		struct vm_area_struct *vma;

		if (atomic)
			return get_fault_pfn();

		down_read(&current->mm->mmap_sem);
		if (npages == -EHWPOISON ||
			(!async && check_user_page_hwpoison(addr))) {
			up_read(&current->mm->mmap_sem);
			get_page(hwpoison_page);
			return page_to_pfn(hwpoison_page);
		}

		vma = find_vma_intersection(current->mm, addr, addr+1);

		if (vma == NULL)
			pfn = get_fault_pfn();
		else if ((vma->vm_flags & VM_PFNMAP)) {
			pfn = ((addr - vma->vm_start) >> PAGE_SHIFT) +
				vma->vm_pgoff;
			BUG_ON(!kvm_is_mmio_pfn(pfn));
		} else {
			if (async && (vma->vm_flags & VM_WRITE))
				*async = true;
			pfn = get_fault_pfn();
		}
		up_read(&current->mm->mmap_sem);
	} else
		pfn = page_to_pfn(page[0]);

	return pfn;
}