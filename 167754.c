static void remove_huge_page(struct page *page)
{
	ClearPageDirty(page);
	ClearPageUptodate(page);
	delete_from_page_cache(page);
}