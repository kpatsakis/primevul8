static int kvm_vcpu_fault(struct vm_area_struct *vma, struct vm_fault *vmf)
{
	struct kvm_vcpu *vcpu = vma->vm_file->private_data;
	struct page *page;

	if (vmf->pgoff == 0)
		page = virt_to_page(vcpu->run);
#ifdef CONFIG_X86
	else if (vmf->pgoff == KVM_PIO_PAGE_OFFSET)
		page = virt_to_page(vcpu->arch.pio_data);
#endif
#ifdef KVM_COALESCED_MMIO_PAGE_OFFSET
	else if (vmf->pgoff == KVM_COALESCED_MMIO_PAGE_OFFSET)
		page = virt_to_page(vcpu->kvm->coalesced_mmio_ring);
#endif
	else
		return VM_FAULT_SIGBUS;
	get_page(page);
	vmf->page = page;
	return 0;
}