static inline void set_kthread_struct(void *kthread)
{
	/*
	 * We abuse ->set_child_tid to avoid the new member and because it
	 * can't be wrongly copied by copy_process(). We also rely on fact
	 * that the caller can't exec, so PF_KTHREAD can't be cleared.
	 */
	current->set_child_tid = (__force void __user *)kthread;
}