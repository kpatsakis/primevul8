static void put_cred_rcu(struct rcu_head *rcu)
{
	struct cred *cred = container_of(rcu, struct cred, rcu);

	kdebug("put_cred_rcu(%p)", cred);

#ifdef CONFIG_DEBUG_CREDENTIALS
	if (cred->magic != CRED_MAGIC_DEAD ||
	    atomic_read(&cred->usage) != 0 ||
	    read_cred_subscribers(cred) != 0)
		panic("CRED: put_cred_rcu() sees %p with"
		      " mag %x, put %p, usage %d, subscr %d\n",
		      cred, cred->magic, cred->put_addr,
		      atomic_read(&cred->usage),
		      read_cred_subscribers(cred));
#else
	if (atomic_read(&cred->usage) != 0)
		panic("CRED: put_cred_rcu() sees %p with usage %d\n",
		      cred, atomic_read(&cred->usage));
#endif

	security_cred_free(cred);
	key_put(cred->session_keyring);
	key_put(cred->process_keyring);
	key_put(cred->thread_keyring);
	key_put(cred->request_key_auth);
	if (cred->group_info)
		put_group_info(cred->group_info);
	free_uid(cred->user);
	put_user_ns(cred->user_ns);
	kmem_cache_free(cred_jar, cred);
}