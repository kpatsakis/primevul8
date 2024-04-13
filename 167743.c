static inline void hugetlb_drop_vma_policy(struct vm_area_struct *vma)
{
	mpol_cond_put(vma->vm_policy);
}