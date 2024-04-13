static int kvm_vm_fault(struct vm_area_struct *vma, struct vm_fault *vmf)
{
	struct page *page[1];
	unsigned long addr;
	int npages;
	gfn_t gfn = vmf->pgoff;
	struct kvm *kvm = vma->vm_file->private_data;

	addr = gfn_to_hva(kvm, gfn);
	if (kvm_is_error_hva(addr))
		return VM_FAULT_SIGBUS;

	npages = get_user_pages(current, current->mm, addr, 1, 1, 0, page,
				NULL);
	if (unlikely(npages != 1))
		return VM_FAULT_SIGBUS;

	vmf->page = page[0];
	return 0;
}