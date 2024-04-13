static inline struct kthread *to_kthread(struct task_struct *k)
{
	WARN_ON(!(k->flags & PF_KTHREAD));
	return (__force void *)k->set_child_tid;
}