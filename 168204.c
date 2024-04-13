static inline void load_mm_cr4(struct mm_struct *mm)
{
	if (static_key_false(&rdpmc_always_available) ||
	    atomic_read(&mm->context.perf_rdpmc_allowed))
		cr4_set_bits(X86_CR4_PCE);
	else
		cr4_clear_bits(X86_CR4_PCE);
}