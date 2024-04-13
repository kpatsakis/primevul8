int is_hwpoison_pfn(pfn_t pfn)
{
	return pfn == hwpoison_pfn;
}