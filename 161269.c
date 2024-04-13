struct cred *prepare_exec_creds(void)
{
	struct cred *new;

	new = prepare_creds();
	if (!new)
		return new;

#ifdef CONFIG_KEYS
	/* newly exec'd tasks don't get a thread keyring */
	key_put(new->thread_keyring);
	new->thread_keyring = NULL;

	/* inherit the session keyring; new process keyring */
	key_put(new->process_keyring);
	new->process_keyring = NULL;
#endif

	return new;
}