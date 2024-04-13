static pfn_t get_fault_pfn(void)
{
	get_page(fault_page);
	return fault_pfn;
}