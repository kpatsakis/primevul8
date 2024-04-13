static int selinux_task_setioprio(struct task_struct *p, int ioprio)
{
	int rc;

	rc = cap_task_setioprio(p, ioprio);
	if (rc)
		return rc;

	return current_has_perm(p, PROCESS__SETSCHED);
}