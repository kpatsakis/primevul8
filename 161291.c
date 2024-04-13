void __put_cred(struct cred *cred)
{
	kdebug("__put_cred(%p{%d,%d})", cred,
	       atomic_read(&cred->usage),
	       read_cred_subscribers(cred));

	BUG_ON(atomic_read(&cred->usage) != 0);
#ifdef CONFIG_DEBUG_CREDENTIALS
	BUG_ON(read_cred_subscribers(cred) != 0);
	cred->magic = CRED_MAGIC_DEAD;
	cred->put_addr = __builtin_return_address(0);
#endif
	BUG_ON(cred == current->cred);
	BUG_ON(cred == current->real_cred);

	call_rcu(&cred->rcu, put_cred_rcu);
}