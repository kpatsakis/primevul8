void __init parse_efi_setup(u64 phys_addr, u32 data_len)
{
	efi_setup = phys_addr + sizeof(struct setup_data);
}