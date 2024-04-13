int __init efi_alloc_page_tables(void)
{
	pgd_t *pgd, *efi_pgd;
	p4d_t *p4d;
	pud_t *pud;
	gfp_t gfp_mask;

	if (efi_enabled(EFI_OLD_MEMMAP))
		return 0;

	gfp_mask = GFP_KERNEL | __GFP_ZERO;
	efi_pgd = (pgd_t *)__get_free_pages(gfp_mask, PGD_ALLOCATION_ORDER);
	if (!efi_pgd)
		return -ENOMEM;

	pgd = efi_pgd + pgd_index(EFI_VA_END);
	p4d = p4d_alloc(&init_mm, pgd, EFI_VA_END);
	if (!p4d) {
		free_page((unsigned long)efi_pgd);
		return -ENOMEM;
	}

	pud = pud_alloc(&init_mm, p4d, EFI_VA_END);
	if (!pud) {
		if (pgtable_l5_enabled())
			free_page((unsigned long) pgd_page_vaddr(*pgd));
		free_pages((unsigned long)efi_pgd, PGD_ALLOCATION_ORDER);
		return -ENOMEM;
	}

	efi_mm.pgd = efi_pgd;
	mm_init_cpumask(&efi_mm);
	init_new_context(NULL, &efi_mm);

	return 0;
}