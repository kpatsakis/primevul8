void __init efi_dump_pagetable(void)
{
#ifdef CONFIG_EFI_PGT_DUMP
	if (efi_enabled(EFI_OLD_MEMMAP))
		ptdump_walk_pgd_level(NULL, swapper_pg_dir);
	else
		ptdump_walk_pgd_level(NULL, efi_mm.pgd);
#endif
}