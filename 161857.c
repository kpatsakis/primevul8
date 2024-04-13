static int selinux_capable(const struct cred *cred, struct user_namespace *ns,
			   int cap, int audit)
{
	int rc;

	rc = cap_capable(cred, ns, cap, audit);
	if (rc)
		return rc;

	return cred_has_capability(cred, cap, audit);
}