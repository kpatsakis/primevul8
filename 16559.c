static void unlock_trace(struct task_struct *task)
{
	mutex_unlock(&task->signal->cred_guard_mutex);
}