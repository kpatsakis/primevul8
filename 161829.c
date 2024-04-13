static int selinux_task_setnice(struct task_struct *p, int nice)
{
	int rc;

	rc = cap_task_setnice(p, nice);
	if (rc)
		return rc;

	return current_has_perm(p, PROCESS__SETSCHED);
}