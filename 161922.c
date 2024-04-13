static int __init efi_update_mappings(efi_memory_desc_t *md, unsigned long pf)
{
	unsigned long pfn;
	pgd_t *pgd = efi_mm.pgd;
	int err1, err2;

	/* Update the 1:1 mapping */
	pfn = md->phys_addr >> PAGE_SHIFT;
	err1 = kernel_map_pages_in_pgd(pgd, pfn, md->phys_addr, md->num_pages, pf);
	if (err1) {
		pr_err("Error while updating 1:1 mapping PA 0x%llx -> VA 0x%llx!\n",
			   md->phys_addr, md->virt_addr);
	}

	err2 = kernel_map_pages_in_pgd(pgd, pfn, md->virt_addr, md->num_pages, pf);
	if (err2) {
		pr_err("Error while updating VA mapping PA 0x%llx -> VA 0x%llx!\n",
			   md->phys_addr, md->virt_addr);
	}

	return err1 || err2;
}