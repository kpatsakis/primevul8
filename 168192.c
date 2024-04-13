static inline void arch_bprm_mm_init(struct mm_struct *mm,
		struct vm_area_struct *vma)
{
	mpx_mm_init(mm);
}