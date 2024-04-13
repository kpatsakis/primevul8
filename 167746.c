static void remove_inode_hugepages(struct inode *inode, loff_t lstart,
				   loff_t lend)
{
	struct hstate *h = hstate_inode(inode);
	struct address_space *mapping = &inode->i_data;
	const pgoff_t start = lstart >> huge_page_shift(h);
	const pgoff_t end = lend >> huge_page_shift(h);
	struct vm_area_struct pseudo_vma;
	struct pagevec pvec;
	pgoff_t next;
	int i, freed = 0;
	long lookup_nr = PAGEVEC_SIZE;
	bool truncate_op = (lend == LLONG_MAX);

	memset(&pseudo_vma, 0, sizeof(struct vm_area_struct));
	pseudo_vma.vm_flags = (VM_HUGETLB | VM_MAYSHARE | VM_SHARED);
	pagevec_init(&pvec, 0);
	next = start;
	while (next < end) {
		/*
		 * Don't grab more pages than the number left in the range.
		 */
		if (end - next < lookup_nr)
			lookup_nr = end - next;

		/*
		 * When no more pages are found, we are done.
		 */
		if (!pagevec_lookup(&pvec, mapping, next, lookup_nr))
			break;

		for (i = 0; i < pagevec_count(&pvec); ++i) {
			struct page *page = pvec.pages[i];
			u32 hash;

			/*
			 * The page (index) could be beyond end.  This is
			 * only possible in the punch hole case as end is
			 * max page offset in the truncate case.
			 */
			next = page->index;
			if (next >= end)
				break;

			hash = hugetlb_fault_mutex_hash(h, current->mm,
							&pseudo_vma,
							mapping, next, 0);
			mutex_lock(&hugetlb_fault_mutex_table[hash]);

			lock_page(page);
			if (likely(!page_mapped(page))) {
				bool rsv_on_error = !PagePrivate(page);
				/*
				 * We must free the huge page and remove
				 * from page cache (remove_huge_page) BEFORE
				 * removing the region/reserve map
				 * (hugetlb_unreserve_pages).  In rare out
				 * of memory conditions, removal of the
				 * region/reserve map could fail.  Before
				 * free'ing the page, note PagePrivate which
				 * is used in case of error.
				 */
				remove_huge_page(page);
				freed++;
				if (!truncate_op) {
					if (unlikely(hugetlb_unreserve_pages(
							inode, next,
							next + 1, 1)))
						hugetlb_fix_reserve_counts(
							inode, rsv_on_error);
				}
			} else {
				/*
				 * If page is mapped, it was faulted in after
				 * being unmapped.  It indicates a race between
				 * hole punch and page fault.  Do nothing in
				 * this case.  Getting here in a truncate
				 * operation is a bug.
				 */
				BUG_ON(truncate_op);
			}

			unlock_page(page);
			mutex_unlock(&hugetlb_fault_mutex_table[hash]);
		}
		++next;
		huge_pagevec_release(&pvec);
		cond_resched();
	}

	if (truncate_op)
		(void)hugetlb_unreserve_pages(inode, start, LONG_MAX, freed);
}