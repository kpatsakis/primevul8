static int __init arch_parse_efi_cmdline(char *str)
{
	if (!str) {
		pr_warn("need at least one option\n");
		return -EINVAL;
	}

	if (parse_option_str(str, "old_map"))
		set_bit(EFI_OLD_MEMMAP, &efi.flags);

	return 0;
}