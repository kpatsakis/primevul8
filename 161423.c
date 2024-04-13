void free_kthread_struct(struct task_struct *k)
{
	/*
	 * Can be NULL if this kthread was created by kernel_thread()
	 * or if kmalloc() in kthread() failed.
	 */
	kfree(to_kthread(k));
}