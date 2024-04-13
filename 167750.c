static inline void hugetlb_set_vma_policy(struct vm_area_struct *vma,
					struct inode *inode, pgoff_t index)
{
	vma->vm_policy = mpol_shared_policy_lookup(&HUGETLBFS_I(inode)->policy,
							index);
}