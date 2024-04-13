static int selinux_task_setscheduler(struct task_struct *p)
{
	int rc;

	rc = cap_task_setscheduler(p);
	if (rc)
		return rc;

	return current_has_perm(p, PROCESS__SETSCHED);
}