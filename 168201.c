static inline void arch_exit_mmap(struct mm_struct *mm)
{
	paravirt_arch_exit_mmap(mm);
}