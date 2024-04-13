int install_session_keyring_to_cred(struct cred *cred, struct key *keyring)
{
	unsigned long flags;
	struct key *old;

	might_sleep();

	/* create an empty session keyring */
	if (!keyring) {
		flags = KEY_ALLOC_QUOTA_OVERRUN;
		if (cred->session_keyring)
			flags = KEY_ALLOC_IN_QUOTA;

		keyring = keyring_alloc("_ses", cred->uid, cred->gid,
					cred, flags, NULL);
		if (IS_ERR(keyring))
			return PTR_ERR(keyring);
	} else {
		atomic_inc(&keyring->usage);
	}

	/* install the keyring */
	old = cred->session_keyring;
	rcu_assign_pointer(cred->session_keyring, keyring);

	if (old)
		key_put(old);

	return 0;
}