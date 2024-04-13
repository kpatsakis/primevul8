static long cmm_free_pages(long nr, long *counter, struct cmm_page_array **list)
{
	struct cmm_page_array *pa;
	unsigned long addr;

	spin_lock(&cmm_lock);
	pa = *list;
	while (nr) {
		if (!pa || pa->index <= 0)
			break;
		addr = pa->pages[--pa->index];
		if (pa->index == 0) {
			pa = pa->next;
			free_page((unsigned long) *list);
			*list = pa;
		}
		free_page(addr);
		(*counter)--;
		nr--;
	}
	spin_unlock(&cmm_lock);
	return nr;
}