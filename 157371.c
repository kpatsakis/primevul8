int bnxt_re_mmap(struct ib_ucontext *ib_uctx, struct vm_area_struct *vma)
{
	struct bnxt_re_ucontext *uctx = container_of(ib_uctx,
						   struct bnxt_re_ucontext,
						   ib_uctx);
	struct bnxt_re_dev *rdev = uctx->rdev;
	u64 pfn;

	if (vma->vm_end - vma->vm_start != PAGE_SIZE)
		return -EINVAL;

	if (vma->vm_pgoff) {
		vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
		if (io_remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff,
				       PAGE_SIZE, vma->vm_page_prot)) {
			dev_err(rdev_to_dev(rdev), "Failed to map DPI");
			return -EAGAIN;
		}
	} else {
		pfn = virt_to_phys(uctx->shpg) >> PAGE_SHIFT;
		if (remap_pfn_range(vma, vma->vm_start,
				    pfn, PAGE_SIZE, vma->vm_page_prot)) {
			dev_err(rdev_to_dev(rdev),
				"Failed to map shared page");
			return -EAGAIN;
		}
	}

	return 0;
}