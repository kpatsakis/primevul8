int copy_creds(struct task_struct *p, unsigned long clone_flags)
{
	struct cred *new;
	int ret;

	if (
#ifdef CONFIG_KEYS
		!p->cred->thread_keyring &&
#endif
		clone_flags & CLONE_THREAD
	    ) {
		p->real_cred = get_cred(p->cred);
		get_cred(p->cred);
		alter_cred_subscribers(p->cred, 2);
		kdebug("share_creds(%p{%d,%d})",
		       p->cred, atomic_read(&p->cred->usage),
		       read_cred_subscribers(p->cred));
		atomic_inc(&p->cred->user->processes);
		return 0;
	}

	new = prepare_creds();
	if (!new)
		return -ENOMEM;

	if (clone_flags & CLONE_NEWUSER) {
		ret = create_user_ns(new);
		if (ret < 0)
			goto error_put;
	}

#ifdef CONFIG_KEYS
	/* new threads get their own thread keyrings if their parent already
	 * had one */
	if (new->thread_keyring) {
		key_put(new->thread_keyring);
		new->thread_keyring = NULL;
		if (clone_flags & CLONE_THREAD)
			install_thread_keyring_to_cred(new);
	}

	/* The process keyring is only shared between the threads in a process;
	 * anything outside of those threads doesn't inherit.
	 */
	if (!(clone_flags & CLONE_THREAD)) {
		key_put(new->process_keyring);
		new->process_keyring = NULL;
	}
#endif

	atomic_inc(&new->user->processes);
	p->cred = p->real_cred = get_cred(new);
	alter_cred_subscribers(new, 2);
	validate_creds(new);
	return 0;

error_put:
	put_cred(new);
	return ret;
}