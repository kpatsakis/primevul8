void __init efi_call_phys_epilog(pgd_t *save_pgd)
{
	/*
	 * After the lock is released, the original page table is restored.
	 */
	int pgd_idx, i;
	int nr_pgds;
	pgd_t *pgd;
	p4d_t *p4d;
	pud_t *pud;

	if (!efi_enabled(EFI_OLD_MEMMAP)) {
		efi_switch_mm(efi_scratch.prev_mm);
		return;
	}

	nr_pgds = DIV_ROUND_UP((max_pfn << PAGE_SHIFT) , PGDIR_SIZE);

	for (pgd_idx = 0; pgd_idx < nr_pgds; pgd_idx++) {
		pgd = pgd_offset_k(pgd_idx * PGDIR_SIZE);
		set_pgd(pgd_offset_k(pgd_idx * PGDIR_SIZE), save_pgd[pgd_idx]);

		if (!pgd_present(*pgd))
			continue;

		for (i = 0; i < PTRS_PER_P4D; i++) {
			p4d = p4d_offset(pgd,
					 pgd_idx * PGDIR_SIZE + i * P4D_SIZE);

			if (!p4d_present(*p4d))
				continue;

			pud = (pud_t *)p4d_page_vaddr(*p4d);
			pud_free(&init_mm, pud);
		}

		p4d = (p4d_t *)pgd_page_vaddr(*pgd);
		p4d_free(&init_mm, p4d);
	}

	kfree(save_pgd);

	__flush_tlb_all();
	early_code_mapping_set_exec(0);
}