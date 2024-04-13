static unsigned long efi_name_size(efi_char16_t *name)
{
	return ucs2_strsize(name, EFI_VAR_NAME_LEN) + 1;
}