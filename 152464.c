int kvm_assign_device(struct kvm *kvm,
		      struct kvm_assigned_dev_kernel *assigned_dev)
{
	struct pci_dev *pdev = NULL;
	struct iommu_domain *domain = kvm->arch.iommu_domain;
	int r;
	bool noncoherent;

	/* check if iommu exists and in use */
	if (!domain)
		return 0;

	pdev = assigned_dev->dev;
	if (pdev == NULL)
		return -ENODEV;

	r = iommu_attach_device(domain, &pdev->dev);
	if (r) {
		dev_err(&pdev->dev, "kvm assign device failed ret %d", r);
		return r;
	}

	noncoherent = !iommu_capable(&pci_bus_type, IOMMU_CAP_CACHE_COHERENCY);

	/* Check if need to update IOMMU page table for guest memory */
	if (noncoherent != kvm->arch.iommu_noncoherent) {
		kvm_iommu_unmap_memslots(kvm);
		kvm->arch.iommu_noncoherent = noncoherent;
		r = kvm_iommu_map_memslots(kvm);
		if (r)
			goto out_unmap;
	}

	pci_set_dev_assigned(pdev);

	dev_info(&pdev->dev, "kvm assign device\n");

	return 0;
out_unmap:
	kvm_iommu_unmap_memslots(kvm);
	return r;
}