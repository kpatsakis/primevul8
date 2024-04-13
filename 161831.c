static int selinux_capset(struct cred *new, const struct cred *old,
			  const kernel_cap_t *effective,
			  const kernel_cap_t *inheritable,
			  const kernel_cap_t *permitted)
{
	int error;

	error = cap_capset(new, old,
				      effective, inheritable, permitted);
	if (error)
		return error;

	return cred_has_perm(old, new, PROCESS__SETCAP);
}