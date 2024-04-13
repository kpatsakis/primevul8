static long cmm_alloc_pages(long nr, long *counter,
			    struct cmm_page_array **list)
{
	struct cmm_page_array *pa, *npa;
	unsigned long addr;

	while (nr) {
		addr = __get_free_page(GFP_NOIO);
		if (!addr)
			break;
		spin_lock(&cmm_lock);
		pa = *list;
		if (!pa || pa->index >= CMM_NR_PAGES) {
			/* Need a new page for the page list. */
			spin_unlock(&cmm_lock);
			npa = (struct cmm_page_array *)
				__get_free_page(GFP_NOIO);
			if (!npa) {
				free_page(addr);
				break;
			}
			spin_lock(&cmm_lock);
			pa = *list;
			if (!pa || pa->index >= CMM_NR_PAGES) {
				npa->next = pa;
				npa->index = 0;
				pa = npa;
				*list = pa;
			} else
				free_page((unsigned long) npa);
		}
		diag10_range(addr >> PAGE_SHIFT, 1);
		pa->pages[pa->index++] = addr;
		(*counter)++;
		spin_unlock(&cmm_lock);
		nr--;
	}
	return nr;
}