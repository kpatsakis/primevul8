static int hugetlb_vmtruncate(struct inode *inode, loff_t offset)
{
	pgoff_t pgoff;
	struct address_space *mapping = inode->i_mapping;
	struct hstate *h = hstate_inode(inode);

	BUG_ON(offset & ~huge_page_mask(h));
	pgoff = offset >> PAGE_SHIFT;

	i_size_write(inode, offset);
	i_mmap_lock_write(mapping);
	if (!RB_EMPTY_ROOT(&mapping->i_mmap))
		hugetlb_vmdelete_list(&mapping->i_mmap, pgoff, 0);
	i_mmap_unlock_write(mapping);
	remove_inode_hugepages(inode, offset, LLONG_MAX);
	return 0;
}