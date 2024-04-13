int is_fault_pfn(pfn_t pfn)
{
	return pfn == fault_pfn;
}