const struct cred *override_creds(const struct cred *new)
{
	const struct cred *old = current->cred;

	kdebug("override_creds(%p{%d,%d})", new,
	       atomic_read(&new->usage),
	       read_cred_subscribers(new));

	validate_creds(old);
	validate_creds(new);
	get_cred(new);
	alter_cred_subscribers(new, 1);
	rcu_assign_pointer(current->cred, new);
	alter_cred_subscribers(old, -1);

	kdebug("override_creds() = %p{%d,%d}", old,
	       atomic_read(&old->usage),
	       read_cred_subscribers(old));
	return old;
}