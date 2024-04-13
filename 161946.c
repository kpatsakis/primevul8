void efi_sync_low_kernel_mappings(void)
{
	unsigned num_entries;
	pgd_t *pgd_k, *pgd_efi;
	p4d_t *p4d_k, *p4d_efi;
	pud_t *pud_k, *pud_efi;
	pgd_t *efi_pgd = efi_mm.pgd;

	if (efi_enabled(EFI_OLD_MEMMAP))
		return;

	/*
	 * We can share all PGD entries apart from the one entry that
	 * covers the EFI runtime mapping space.
	 *
	 * Make sure the EFI runtime region mappings are guaranteed to
	 * only span a single PGD entry and that the entry also maps
	 * other important kernel regions.
	 */
	MAYBE_BUILD_BUG_ON(pgd_index(EFI_VA_END) != pgd_index(MODULES_END));
	MAYBE_BUILD_BUG_ON((EFI_VA_START & PGDIR_MASK) !=
			(EFI_VA_END & PGDIR_MASK));

	pgd_efi = efi_pgd + pgd_index(PAGE_OFFSET);
	pgd_k = pgd_offset_k(PAGE_OFFSET);

	num_entries = pgd_index(EFI_VA_END) - pgd_index(PAGE_OFFSET);
	memcpy(pgd_efi, pgd_k, sizeof(pgd_t) * num_entries);

	/*
	 * As with PGDs, we share all P4D entries apart from the one entry
	 * that covers the EFI runtime mapping space.
	 */
	BUILD_BUG_ON(p4d_index(EFI_VA_END) != p4d_index(MODULES_END));
	BUILD_BUG_ON((EFI_VA_START & P4D_MASK) != (EFI_VA_END & P4D_MASK));

	pgd_efi = efi_pgd + pgd_index(EFI_VA_END);
	pgd_k = pgd_offset_k(EFI_VA_END);
	p4d_efi = p4d_offset(pgd_efi, 0);
	p4d_k = p4d_offset(pgd_k, 0);

	num_entries = p4d_index(EFI_VA_END);
	memcpy(p4d_efi, p4d_k, sizeof(p4d_t) * num_entries);

	/*
	 * We share all the PUD entries apart from those that map the
	 * EFI regions. Copy around them.
	 */
	BUILD_BUG_ON((EFI_VA_START & ~PUD_MASK) != 0);
	BUILD_BUG_ON((EFI_VA_END & ~PUD_MASK) != 0);

	p4d_efi = p4d_offset(pgd_efi, EFI_VA_END);
	p4d_k = p4d_offset(pgd_k, EFI_VA_END);
	pud_efi = pud_offset(p4d_efi, 0);
	pud_k = pud_offset(p4d_k, 0);

	num_entries = pud_index(EFI_VA_END);
	memcpy(pud_efi, pud_k, sizeof(pud_t) * num_entries);

	pud_efi = pud_offset(p4d_efi, EFI_VA_START);
	pud_k = pud_offset(p4d_k, EFI_VA_START);

	num_entries = PTRS_PER_PUD - pud_index(EFI_VA_START);
	memcpy(pud_efi, pud_k, sizeof(pud_t) * num_entries);
}