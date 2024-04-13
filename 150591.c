int is_error_page(struct page *page)
{
	return page == bad_page || page == hwpoison_page || page == fault_page;
}