static long hugetlbfs_punch_hole(struct inode *inode, loff_t offset, loff_t len)
{
	struct hstate *h = hstate_inode(inode);
	loff_t hpage_size = huge_page_size(h);
	loff_t hole_start, hole_end;

	/*
	 * For hole punch round up the beginning offset of the hole and
	 * round down the end.
	 */
	hole_start = round_up(offset, hpage_size);
	hole_end = round_down(offset + len, hpage_size);

	if (hole_end > hole_start) {
		struct address_space *mapping = inode->i_mapping;

		mutex_lock(&inode->i_mutex);
		i_mmap_lock_write(mapping);
		if (!RB_EMPTY_ROOT(&mapping->i_mmap))
			hugetlb_vmdelete_list(&mapping->i_mmap,
						hole_start >> PAGE_SHIFT,
						hole_end  >> PAGE_SHIFT);
		i_mmap_unlock_write(mapping);
		remove_inode_hugepages(inode, hole_start, hole_end);
		mutex_unlock(&inode->i_mutex);
	}

	return 0;
}