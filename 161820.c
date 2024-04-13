static int selinux_capget(struct task_struct *target, kernel_cap_t *effective,
			  kernel_cap_t *inheritable, kernel_cap_t *permitted)
{
	int error;

	error = current_has_perm(target, PROCESS__GETCAP);
	if (error)
		return error;

	return cap_capget(target, effective, inheritable, permitted);
}