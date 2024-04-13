int is_error_pfn(pfn_t pfn)
{
	return pfn == bad_pfn || pfn == hwpoison_pfn || pfn == fault_pfn;
}