static struct mm_struct *__check_mem_permission(struct task_struct *task)
{
	struct mm_struct *mm;

	mm = get_task_mm(task);
	if (!mm)
		return ERR_PTR(-EINVAL);

	/*
	 * A task can always look at itself, in case it chooses
	 * to use system calls instead of load instructions.
	 */
	if (task == current)
		return mm;

	/*
	 * If current is actively ptrace'ing, and would also be
	 * permitted to freshly attach with ptrace now, permit it.
	 */
	if (task_is_stopped_or_traced(task)) {
		int match;
		rcu_read_lock();
		match = (ptrace_parent(task) == current);
		rcu_read_unlock();
		if (match && ptrace_may_access(task, PTRACE_MODE_ATTACH))
			return mm;
	}

	/*
	 * No one else is allowed.
	 */
	mmput(mm);
	return ERR_PTR(-EPERM);
}